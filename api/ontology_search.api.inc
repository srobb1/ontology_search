<?php
/**
 * @file
 * Provides an application programming interface (API) for Ontology Search
 */

/**
 * @defgroup ontology_search_api Ontology Search API
 * @ingroup tripal_api
 * @{
 *   Provides an application programming interface (API) for working with the Ontology Search module
 * @}
 */

class TermNode
{
  public $objectId;
  public $pathLength;
  public $relativeType;
  public $name;
  
  public $next;
  function __construct($objectId,$pathLength,$type) {
    $this->objectId = $objectId;
    $this->pathLength = $pathLength;
    $this->relativeType = $type;
    $this->name= NULL;
    $this->next = NULL;
  }
  function setName($nam) {
    $this->name = $nam;
  }
  function getName() {
    return $this->name;
  }
  function readNode() {
      return $this->objectId;
  }
  function getPathlength() {
      return $this->pathLength;
  }
  function getType() {
      return $this->relativeType;
  }
}


/**
 * ontology_search_getParent()
 *
 * Gets the names of the ontology ids in $avail.
 *
 * @param $id
 *   cvterm.cvterm_id to search with
 *
 * @ingroup ontology_search
 */
function ontology_search_getParent($id) {
//EKSC: why retrieve $prefix here?
  $prefix = $form_state['values']['name'];

  $count = 0;
  if ($GLOBALS["chado_is_installed"]) {
    if (!function_exists('chado_query')) {
      drupal_set_message("problem with chado_query");
    }
    
    $sql = "SELECT cp.object_id, c.name AS relationship, cp.pathdistance 
            FROM {cvtermpath} cp
              INNER JOIN cvterm c ON c.cvterm_id=cp.type_id
            WHERE subject_id = :subject_id 
            ORDER BY pathdistance ASC";
//drupal_set_message("ontology_search_getParent(): search with [$id]");
    $results = chado_query($sql, array(':subject_id' => $id));
//drupal_set_message("ontology_search_getParent(): Query results: <pre>" . print_r($results, TRUE) . "</pre>");

    $sta = new \SplStack();
    foreach ($results as $record) {
//drupal_set_message("ontology_search_getParent(): one result record: <pre>" . print_r($record, TRUE) . "</pre>");
      $count++;
      $n = new TermNode($record->object_id, 
                        $record->pathdistance, 
                        $record->relationship);
      $sta->push($n);
    }
    if ($sta->isEmpty())
      return NULL;
     
    $out = $sta->pop();
    $head = $out;
    $tail = $out;
    while (!$sta->isEmpty()) {
       $out = $sta->pop();
       $tail->next = $out;
       $tail = $tail->next;
    }
    $temp = $head;
    while ($temp != null) {
      $cv = $temp->readNode();
      $pl = $temp->getPathlength();
      $sql = "SELECT name FROM cvterm WHERE cvterm_id = :cvterm_id";
      $resu = db_query($sql, array(':cvterm_id' => $temp->readNode()));
      foreach ($resu as $record) {
         $temp->setName($record->name);
      }
      $temp = $temp->next;

    }
    return $head;
  }
  
//EKSC: need to return something to indicate the search failed. Returning
//      false if often the best solution.
}


/**
 * get_Names_Ontologies_Available()
 *
 * Gets the names of the ontology ids in $avail.
 *
 * @param $avail
 *   An array of cv.cv_id values
 *
 * @ingroup ontology_search
 */
function get_Names_Ontologies_Available($avail) { 
  $final = array();
  
  foreach ($avail as $key=>$value){
    $sql =  "SELECT name FROM {cv} WHERE cv_id = :cv_id ";
    $results = chado_query($sql, array(':cv_id' => $key));
    foreach ($results as $record) {
      $final[$key] = $record->name; 
    }
//drupal_set_message("Key=".$key." value=".$value);
  }

  return $final;
}


/**
 * ontology_search_setSimilarTerms()
 *
 * Find cvterms in the selected cv's that are similar to the given term.
 *
 * @param $prefix
 *   The term to search for
 *
 * @ingroup ontology_search
 */
//EKSC: Should search for the term anywhere in the name, not just the beginning.
//      Then the parameter should be named "term", not "prefix".
//EKSC: pass in $OntologiesLoaded. Remember that this is a generic function
//      that may be called outside the module, or even from outside Drupal, so
//      don't assume the existance of a drupal or session variable.
function ontology_search_setSimilarTerms($prefix, $OntologiesLoaded) {
  $similarTerms = false;  // false unless successfully populated.
  
  if ($prefix != NULL) { 
//EKSC: Should fail with an error if chado not installed or if chado_query 
//      doesn't exist. Good to test for missing requirements, but you need to 
//      report when they are missing. Code should "fail gracefully" so it is
//      immediately apparent something is wrong.
    if ($GLOBALS["chado_is_installed"]) {
      if (function_exists('chado_query')) {
//drupal_set_message("Chado exists");
      }
       
//EKSC: more efficient to modify this query to only search specific cv_ids.
//      use the IN clause: http://www.postgresqltutorial.com/postgresql-in/
      $similarTerms = null;
      $results = db_query(
        'SELECT name,cvterm_id,cv_id,dbxref_id FROM cvterm WHERE name LIKE :name',
        array(':name' => '%'.db_like($prefix).'%')
      )->fetchAll();
//drupal_set_message('_setSimilarTerms(): query results: <pre>' . print_r($results, true) . '</pre>');

//drupal_set_message('_setSimilarTerms(): ontologies to search: <pre>' . print_r($OntologiesLoaded, true) . '</pre>');

//EKSC: use SQL to filter for ontologies. See above.
      if ($similarTerms == null) {
        foreach ($results as $r) {
          $rhs = null;
          if (array_key_exists($r->cv_id, $OntologiesLoaded)) {
            $Ont_Nam = $OntologiesLoaded[$r->cv_id];
            $rhs ="$r->name,            $Ont_Nam,               $r->dbxref_id";
            $similarTerms[$r->cvterm_id] = $rhs;
          }
        }
      }
//drupal_set_message('_setSimilarTerms(): found these similar terms: <pre>' . print_r($similarTerms, true) . '</pre>');

//EKSC: again, because this is a general-purpose function it shouldn't assume 
//      the session object exists. Return the array instead.
//      //just in case u want to check if all the similar terms are set in the session
//      unset($_SESSION['similarTerms']);
//      $_SESSION['similarTerms'] = $similarTerms;
    }
  }
  
  return $similarTerms;
}


/**
 * list2string()
 *
 * Traverse a node list into a human-readable string.
 *
 * @param $head
 *   head of the node list
 * @param $relative_type
 *   
 *
 * @ingroup ontology_search
 */
function list2array($head, $relative_type) { 
  if ($head ==NULL) { 
    drupal_set_message("Your Search term does not have any ".$relative_type." !");
    return;
  }
  
  $found_list = array();
  
  $temp = $head;
  while ($temp !=null) {
//drupal_set_message("One node: <pre>" . print_r($temp, true) . '</pre>');
    array_push($found_list, array(
      'cvterm_id' => $temp->objectId,
      'name'      => $temp->name,
      'degree'    => $temp->pathLength,
    ));
    
    $temp = $temp->next; 
  }
  
//EKSC: want the original term at the top, the most distant relative at the 
//      bottom, but this reverse function isn't doing the job. May have something
//      to do with PHP treating array indices the same as keys, e.g. all arrays
//      are hashes.
  // list starts with further-most relative, so reverse it:
  $list_array = array_reverse($found_list, true);
//drupal_set_message("list2array(): The relatives list: <pre>" . print_r($found_list, true) . '</pre>');

  return $found_list;
}