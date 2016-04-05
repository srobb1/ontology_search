<?php

/**
 * hook_form()
 *
 * Implements hook_form.
 * Creates a form for search selected ontologies.
 *
 * @ingroup ontology_search
 */
function ontology_search_form() {
  $form = array();
  
  // Get term searched for, if any
  if (isset($_SESSION['term'])) {
    $term = $_SESSION['term'];
  }
  else {
    $term = '';
  }
  

  $form['some_text'] = array(
    '#markup' => '<p>ONTOLOGY SEARCH</p>'
  );
  
  $form['some_text2'] = array(
    '#markup' => '<p>Please type a word or phrase, then select a specific term to search for:</p>'
  );

  $form['name']=array(
    '#type' => 'textfield',
    '#title' => t('Enter your search term here'),
    '#default_value' => $term,
  );


  // Get the cv ids for ontologies to search
  $vset = variable_get('Ontologies_Available');
drupal_set_message("CV ids to search: <pre>". print_r($vset, TRUE) .'</pre>');

  // Get the names for the cv ids
  $OntologiesLoaded = get_Names_Ontologies_Available($vset);
//drupal_set_message('Ontology names: <pre>'. print_r($OntologiesLoaded, TRUE) .'</pre>');

$form['Ontologies_Avaliable'] = array(
    '#type' => 'select',
    '#title' => t('ontologies available:'),
    '#options' => $OntologiesLoaded,
    '#default_value' => '1000',
    '#description' => t('You can further be specific about the ontology you want to search from'),
  );

drupal_add_css('.ds-2col-fluid >.group-left {float: left; width:auto;}
.ds-2col-fluid > .group-right { float: left; width: 75%; margin-left:15px;}'
, array('group' => CSS_THEME, 'type' => 'inline'));
  
  $form['submit']=array(
    '#type' => 'submit',
    '#value' => t('Search'),
    '#submit' => array('ontology_search_form_submit'),
  );

//EKSC: use drupal variables here instead of a session object
  $similarTerms = $_SESSION['similarTerms'];
  $target_cvterm = $_SESSION['target_cvterm'];
  $form['search_term'] = array(
    '#type' => 'select',
    '#title' => t('Select the specific term of your interest:'),
    '#options' => $similarTerms,
    '#default_value' => $target_cvterm,
    '#description' => t('which specific term are you looking for'),
  );

  
 
  $relative = array(
      'getParent'    => 'getParent',
      'getChildren'  => 'getChildren',
      'getSibilings' => 'getSibilings');
//EKSC: What is this?
//  $form['ALG']['GParam']['GW'] = array(
//EKSC: More clear:
  $form['relationship'] = array(
    '#type' => 'select',
    '#title' => t('Which Relatives:'),
    '#options' => $relative,
    '#default_value' => '',
    '#description' => t('Which relative of the selected term do you want'),
  );
/*EKSC: not necessary to use AJAX, instead create a separate submit function
        ontology_search_relatives_submit()
  $form['query']['example_sequence'] = array(
    '#type' => 'button',
    '#value' => t("Get Relative Terms"),
    '#ajax' => array(
      'callback' =>'Ontology_Search_Get_Relatives',
      'wrapper' => 'dynamic-tags-fieldset',
      'method' => 'replace',
      'event' => 'click',
      'prevent' => 'submit click mousedown',
     ),
    '#limit_validation_errors' => array(), // No elements will be validated
    '#submit' => array('custom_function'), // An explicit submit functions array is required for this, but may be empty
  );
*/

//EKSC: disable this button if there are no similarTerms.
  $form['get_relatives'] = array(
    '#type' => 'submit',
    '#value' => t("Get Relative Terms"),
    '#submit' => array('ontology_search_relatives_submit'),
  );
  
  $form['clear'] = array(
    '#type' => 'submit',
    '#value' => t("Clear search"),
    '#submit' => array('ontology_search_clear_submit'),
  );
 
  // If there are any cvterms, create html to display them
  $html = '';
  $found_list = unserialize(variable_get('FoundTerms'));
  $html="<table >";
  if($found_list !=null)
    $html .= "<tr  ><td>[Degree] </td><td>Name</td><td>Db_Xref</td><td>Description</td><td>Ontology Name</td></tr>" ;
  foreach ($found_list as $cvterm) {
//drupal_set_message("ontology_search_form(): Found this cvterm: <pre>" . print_r($cvterm, true) . '</pre>');
    $html .= "<tr  ><td>[". $cvterm['degree'] . "] </td><td>" . $cvterm['name']."</td><td>".$cvterm['Xref']."</td><td>".$cvterm['def']."</td><td>"."[".$OntologiesLoaded[$cvterm['db_id']]."]</td></tr>" ;
  }
  $html.="</table>";
  if ($html == '<table></table>') {
    $html = 'none';
  }
  $html = "<br><b>Found these terms:</b><br>$html";
  $form['found_terms'] = array(
    '#markup' => $html,
  );
  
return $form;
}


/**
 * hook_form_validate()
 *
 * Implements hook_form_validate.
 * Check ontology search form submission
 *
 * @ingroup ontology_search
 */
function ontology_search_form_validate($form, $form_state) {
//EKSC: 'name' is another example of a misleading variable name. This is the
//      term that is being searched for, it is not the name of anything.
  if (empty($form_state['values']['name'])) {
     form_set_error('name','Name cannot be empty'); 
  }
  else {
    // keep the search term for future reference
    $_SESSION['term'] = $form_state['values']['name'];
  }

  $_SESSION['target_cvterm'] = $form_state['values']['search_term'];
}


/**
 * ontology_search_form_submit()
 *
 * Implements the initial term search submission.
 *
 * @ingroup ontology_search
 */
function ontology_search_form_submit($form, $form_state) {
//drupal_set_message("ontology_search_form_submit(): form contents: <pre>" . print_r($form_state, TRUE) . "</pre>");

  $prefix = $form_state['values']['name'];

  $vset = variable_get('Ontologies_Available');
  $OntologiesLoaded = get_Names_Ontologies_Available($vset);

  // Clear found cvterms list sine we're starting a new search
  variable_del('FoundTerms');
  
//EKSC: Recommend passing in $OntologiesLoaded since you have it already.
//      Also, this function should return the list of terms.

 // drupal_set_message("i am here");
     // drupal_set_message($form_state['values']['Ontologies_Avaliable']);
   $filter_for_a_specific_ont =  $form_state['values']['Ontologies_Avaliable'] ;
  $similarTerms = ontology_search_setSimilarTerms($prefix, $OntologiesLoaded,$filter_for_a_specific_ont);
  
//EKSC: set the similarTerms variable here rather than inside 
//      ontology_search_setSimilarTerms() and use a drupal variable rather
//      than a session variable.
   unset($_SESSION['similarTerms']);
   $_SESSION['similarTerms'] = $similarTerms;
//drupal_set_message('_submit(): found these similar terms: <pre>' . print_r($similarTerms, true) . '</pre>');
}


/**
 * ontology_search_relatives_submit()
 *
 * Implements the relatives search submission.
 *
 * @ingroup ontology_search
 */
function ontology_search_relatives_submit($form, $form_state) {
//drupal_set_message("ontology_search_relatives_submit(): form contents: <pre>" . print_r($form_state, TRUE) . "</pre>");
  $found_terms = false;  // false until/unless matching terms are found
  
  if (isset($form_state['values']['search_term']) && $form_state['values']['search_term'] != '') {
    $cvterm = $form_state['values']['search_term'];
    $relationship = $form_state['values']['relationship'];

drupal_set_message("ontology_search_relatives_submit(): Search $relationship for $cvterm");
    switch ($relationship) {
      case 'getParent':
        $found_terms = ontology_search_getParent($cvterm);
        $found_list = list2array($found_terms, 'getParent');
        break;

       case 'getChildren':
        $found_terms = ontology_search_getChild($cvterm);
        $found_list = list2array($found_terms, 'getChild');
        break; 

        case 'getSibilings':
        $found_terms = ontology_search_getSibilings($cvterm);
        $found_list = list2array($found_terms, 'getSibilings');
        break; 
      
//EKSC: continue this for siblings and parents.
    }//switch
  }
  
  // Save this list for future use
  variable_set('FoundTerms', serialize($found_list));
}


/**
 * ontology_search_clear_submit()
 *
 * Implements the relatives clear submission.
 * Clears the search form and variables.
 *
 * @ingroup ontology_search
 */
function ontology_search_clear_submit($form, $form_state) {
  variable_del('FoundTerms');
  
//EKSC: to do: use a Drupal variable for these instead of the session object
  unset($_SESSION['term']);
  unset($_SESSION['similarTerms']);
}


