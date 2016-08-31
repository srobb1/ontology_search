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
  public $db_xref;
  public $db_id;
  public $description;
  public $accession;
  public $next;
  
  function __construct($objectId,$pathLength,$type) {
    $this->objectId = $objectId;
    $this->pathLength = $pathLength;
    $this->relativeType = $type;
    $this->name= NULL;
    $this->next = NULL;
    $this->db_xref=NULL;
    $this->db_id=NULL;
    $this->description=NULL;
    $this->accession=NULL;
   }
  
  function setAcc($a) {
    $this->accession =$a;
   }
  
  function getAcc() {
    return $this->accession;
   }
  
  function setDesc($des) {
    $this->description =$des;
   }
  
  function getDesc() {
    return $this->description;
   }
  
  function setDbid($id) {
    $this->db_id =$id;
   }
  
  function getDbid() {
    return $this->db_id;
  }
  
  function setDbxref($x) {
    $this->db_xref=$x;
  }
  
  function getDbxref() {
    return $this->db_xref;
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

/*
the below two functions
are responsible for getting the 
sibilings for a particular term
*/

function ontology_search_getSibilings($id)
{
  $count=0;
  if ($GLOBALS["chado_is_installed"]) {
     if (!function_exists('chado_query')) {
         drupal_set_message("chado_query doesnt exists");
         return null;
        }
     $sql = "SELECT object_id,type_id,pathdistance FROM cvtermpath WHERE subject_id = :subject_id ORDER BY pathdistance ASC";
     $parents = db_query($sql, array(':subject_id' => $id));
     if($parents==null)
       {
         drupal_set_message("no parents at level 1");
         return null;
       }
     $sta= new \SplStack();
     foreach ($parents as $record) {
         $node= new TermNode($record->object_id,$record->pathdistance,$record->type_id);
         $sta->push($node);
       }
     $allSibilings=NULL;
     if($sta->isEmpty())
         return NULL;      
     $out=$sta->pop();
     while(!$sta->isEmpty())
      { 
       $distanceFromGivenNode = $out->getPathlength();
       /* get only parents at level 1 and then get immediate child for those nodes */
       if($distanceFromGivenNode ==1)
        {
          $nodeId=$out->readNode();
          $childrenAtLevel1=ontology_search_getChildrenatlevel1($nodeId);
          if($allSibilings==NULL)
           {
             $allSibilings=$childrenAtLevel1;
           }
          else
          {
             $temp=$allSibilings;
             while($temp->next!=NULL)
               {
                  $temp = $temp->next;
               }
             $temp->next=$childrenAtLevel1;
          }
           $out=$sta->pop();
        }
       else
        {
          $out=$sta->pop();
        } 
       }
       return $allSibilings;
    }
   else
    {
     drupal_set_message("chado is not installed");
     return null;
    }

 }
// first function for sibilings gets the first parent

function ontology_search_getChildrenatlevel1($id)
{
  $sql =  "SELECT subject_id,type_id,pathdistance FROM cvtermpath WHERE object_id = :object_id ";
  $children = db_query($sql, array(':object_id' => $id));
  if (!$children)
    {
     echo "An error to get the sibilings at level 1.\n";
     exit;
    }
  $stac= new \SplStack();
  foreach ($children as $record) {
    if($record->pathdistance==1)
      {
        $n= new TermNode($record->subject_id,$record->pathdistance,$record->type_id);
        $stac->push($n);
      }
   }    
   if($stac->isEmpty()) 
   {
    echo "Stack Empty --getChildrenatlevel1 \n";
    exit; 
   }      
  $out=$stac->pop();
  $head=$out;
  $tail=$out;
  while(!$stac->isEmpty())
    {
      $out=$stac->pop();
      $tail->next = $out;   
      $tail= $tail->next;
    }  
  $temp=$head;
 /* get all the information related to a child like name,dbxref, defination, 
    db name, accession and make an array and return */
  while($temp != null)
   { 
     $nodeId=$temp->readNode();
     $sql =  "SELECT name,cv_id,dbxref_id,definition FROM cvterm WHERE cvterm_id = :cvterm_id ";
     $name = db_query($sql, array(':cvterm_id' => $nodeId));
     if (!$name) 
       {
         echo "name fetch error.\n";
         exit;
       }
      foreach ($name as $record) {
         $temp->setName($record->name);
         $temp->setDbxref($record->dbxref_id);
         $temp->setDbid($record->cv_id);
         $temp->setDesc($record->definition);
         $sq = "SELECT accession,db_id FROM dbxref WHERE dbxref_id = :dbxref_id";
         $res = db_query($sq, array(':dbxref_id' => $record->dbxref_id));
        foreach($res as $r)
          {
             $sq = "SELECT name FROM db WHERE db_id = :db_id";
             $dbname = db_query($sq, array(':db_id' => $r->db_id))->fetchField();
             $acc_no = "".$dbname.":".$r->accession."";
             $temp->setAcc($acc_no);
         }
       }
      $temp=$temp->next;
     }
   
   return list2array($head, 'getSibilings');
}

/*
this is second part of getSibiling 
*/




/*
 this function returns the children of a particular term by taking the id
*/


function ontology_search_getChild($id)
{
  if ($GLOBALS["chado_is_installed"]) {
    if (!function_exists('chado_query')) {
        drupal_set_message("chado_query doesnt exists");
        exit;
      }
    $sql =  "SELECT subject_id,type_id,pathdistance FROM cvtermpath WHERE object_id = :object_id ORDER BY pathdistance DESC";
    $child = db_query($sql, array(':object_id' => $id));
    if($child==null){
        drupal_set_message("no children");
        return null;
      }
    $sta= new \SplStack();
    foreach ($child as $record) {
        $n= new TermNode($record->subject_id,$record->pathdistance,$record->type_id);
        $sta->push($n);
      }
    if($sta->isEmpty())
          return NULL;   
    $out=$sta->pop();
    $head=$out;
    $tail=$out;
    while(!$sta->isEmpty())
      {
        $out=$sta->pop();
        $tail->next = $out;
        $tail= $tail->next;
      }
    $temp = $head;
    /* for each child get all the details like name, db name, 
       defination, accession etc */
  while($temp!=null)
      {
         $sql= "SELECT name,cv_id,dbxref_id,definition FROM cvterm WHERE cvterm_id = :cvterm_id";
         $resu = db_query($sql, array(':cvterm_id' => $temp->readNode()));
         foreach ($resu as $record)
           {
            $temp->setName($record->name);
            $temp->setDbxref($record->dbxref_id);
            $temp->setDbid($record->cv_id);
            $temp->setDesc($record->definition);
             $sq = "SELECT accession,db_id FROM dbxref WHERE dbxref_id = :dbxref_id";
             $res = db_query($sq, array(':dbxref_id' => $record->dbxref_id));
            foreach($res as $r)
                 {
                     $sq = "SELECT name FROM db WHERE db_id = :db_id";
                     $dbname = db_query($sq, array(':db_id' => $r->db_id))->fetchField();
                      $acc_no = "".$dbname.":".$r->accession."";
                      $temp->setAcc($acc_no);

                 }
           }
         $temp=$temp->next;

      }
   }
   else
   {
     drupal_set_message("No Children Found");
     return null;
   }
  
  return list2array($head, 'getChild');
   
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
 
  if ($GLOBALS["chado_is_installed"]) {
    if (!function_exists('chado_query')) {
      drupal_set_message("problem with chado_query");
    }
    
    $sql = "SELECT cp.object_id, c.name AS relationship, cp.pathdistance 
            FROM {cvtermpath} cp
              INNER JOIN cvterm c ON c.cvterm_id=cp.type_id
            WHERE subject_id = :subject_id 
            ORDER BY pathdistance ASC";

            $Parents = chado_query($sql, array(':subject_id' => $id));
    $sta = new \SplStack();
    foreach ($Parents as $record) {
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
    /*add more details of node like name, defination
    db name, dbxref_id etc.*/
    while ($temp != null) {
      $sql = "SELECT name,cv_id,dbxref_id,definition FROM cvterm WHERE cvterm_id = :cvterm_id";
      $Parents = db_query($sql, array(':cvterm_id' => $temp->readNode()));
      
      foreach ($Parents as $record) {
         $temp->setName($record->name);
         $temp->setDbxref($record->dbxref_id);
         $temp->setDbid($record->cv_id);
         $temp->setDesc($record->definition);
         $sq = "SELECT accession,db_id FROM dbxref WHERE dbxref_id = :dbxref_id";
         $res = db_query($sq, array(':dbxref_id' => $record->dbxref_id));
         foreach($res as $r)
         {
          $sq = "SELECT name FROM db WHERE db_id = :db_id";
          $dbname = db_query($sq, array(':db_id' => $r->db_id))->fetchField();
          $acc_no = "".$dbname.":".$r->accession."";
          $temp->setAcc($acc_no);
         }
         
       }
      $temp = $temp->next;

    }
      return list2array($head, 'getParent');
  }
  else
   {
     drupal_set_message("Chado Not Installed");
   }
    
  return null;

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

  $final['0']="Select All Available Ontologies";

  foreach ($avail as $key=>$value){
    $sql =  "SELECT name FROM {cv} WHERE cv_id = :cv_id ";
    $results = chado_query($sql, array(':cv_id' => $key));
    foreach ($results as $record) {
      $final[$key] = $record->name; 
      $value=$record->name;
    }
    
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


// $SearchTerm is the pattern that we are looking for
// $OntologiesLoaded are the once that have been Selected by the Admin 
// $filterTerms based on the Ontology selected by the User
function ontology_search_setSimilarTerms($searchTerm, $OntologiesLoaded,$filterTerms) {

   if (!$GLOBALS["chado_is_installed"]) {
       drupal_set_message("Chado Not Installed");
       return null;
      }

    if (!function_exists('chado_query')) {
       drupal_set_message("problem with chado_query");
       return null;
      }

   $similarTerms = false;  // false unless successfully populated.       
  
  if ($searchTerm != NULL) { 

       if($filterTerms==0)
       {
        $sql='SELECT name,cvterm_id,cv_id,dbxref_id FROM cvterm WHERE LOWER(name) LIKE :name ';
       }
       else
       {
        $sql='SELECT name,cvterm_id,cv_id,dbxref_id FROM cvterm WHERE LOWER(name) LIKE :name AND cv_id='.$filterTerms.'';
       }
      $similarTerms = null;
      $all= array_keys($OntologiesLoaded);
      $results = db_query(
        $sql,
        array(':name' => '%'.db_like($searchTerm).'%')
      )->fetchAll();
 if ($similarTerms == null) {
        foreach ($results as $r) {
          $rhs = null;
          if (array_key_exists($r->cv_id, $OntologiesLoaded)) {
            $Ont_Nam = $OntologiesLoaded[$r->cv_id];
            $sq = "SELECT db_id,accession FROM dbxref WHERE dbxref_id = :dbxref_id";
            $res = db_query($sq, array(':dbxref_id' => $r->dbxref_id));
            foreach($res as $re)
             {
               $sq = "SELECT name FROM db WHERE db_id = :db_id";
               $dbname = db_query($sq, array(':db_id' => $re->db_id))->fetchField();
               $acc_no = "".$dbname.":".$re->accession."";
               $rhs ="$r->name,\t\t $Ont_Nam,\t\t [$acc_no]";
             }
            
            $similarTerms[$r->cvterm_id] = $rhs;
          }
        }
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
     array_push($found_list, array(
      'cvterm_id' => $temp->objectId,
      'name'      => $temp->name,
      'degree'    => $temp->pathLength,
      'db_id'    => $temp->db_id,
       'Xref'  => $temp->db_xref,
       'def' => $temp->description,
       'accession' => $temp->accession
    ));
    
    $temp = $temp->next;
   
  }
 /* Puts the term on the Top and distant relative at 
    the Bottom for Children and Parent lists */
       $list_array=$found_list;
  if($relative_type=="getParent")
      $list_array = array_reverse($found_list, true);
      return $list_array;
}
