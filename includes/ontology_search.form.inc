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
  if(variable_get('term')!=null)
   {
     $term= variable_get('term');
   }
  else
   {
    $term='';
   }
  $form['Title'] = array(
     '#markup' => '<p>ONTOLOGY SEARCH</p>'
   );
  $form['some_text2'] = array(
    '#markup' => '<p>Please type a word or phrase, then select a specific term to search for:</p>'
  );
  $htm='';
  $form['searchTerm']=array(
    '#type' => 'textfield',
    '#title' => t('Enter your search term here'),
    '#default_value' => $term,
    '#attributes' => array(
    'id' => 'name_field',
   ),
  );
// Get the cv ids for ontologies to search from which is set by the Admin
  $vset = variable_get('Ontologies_Available');
// Get the names for the cv ids
  $OntologiesLoaded = get_Names_Ontologies_Available($vset);
  if (variable_get('Ontology_Selected') != null) {
    $default_ontology_selected = variable_get('Ontology_Selected');
      }
  else {
     $default_ontology_selected = 0;
      }
  $form['Ontologies_Avaliable'] = array(
      '#type' => 'select',
      '#title' => t('ontologies available:'),
      '#options' => $OntologiesLoaded,
      '#default_value' => $default_ontology_selected,
      '#description' => '<p id="description1">You can further be specific about the ontology you want to search from</p>',
      '#attributes' => array(
      'id' => 'ont_avail',
       ),
     );
  $form['submit']=array(
      '#type' => 'submit',
      '#value' => t('Search'),
      '#submit' => array('ontology_search_form_submit'),
      '#attributes' => array(
      'id' => 'sub',
       ),
     );
//EKSC: use drupal variables here instead of a session object
  $similarTerms = variable_get('similarTerms');
  asort($similarTerms,SORT_FLAG_CASE | SORT_NATURAL);
  $target_cvterm = variable_get('target_cvterm');
  $form['search_term'] = array(
      '#type' => 'select',
      '#title' => t('Select the specific term of your interest:'),
      '#options' => $similarTerms,
      '#default_value' => $target_cvterm,
      '#description' => t('which specific term are you looking for'),
     );
  if(variable_get('Relative_Selected') !=null)
      {
        $Relative_Selected= variable_get('Relative_Selected');
      }
  else
      {
        $default_ontology_selected = 0;
      }
  $relative = array(
      'getParent'    => 'getParent',
      'getChildren'  => 'getChildren',
      'getSibilings' => 'getSibilings');
  $form['relationship'] = array(
      '#type' => 'select',
      '#title' => t('Which Relatives:'),
      '#options' => $relative,
      '#default_value' => $Relative_Selected,
      '#description' => t('Which relative of the selected term do you want'),
     );

  $form['get_relatives'] = array(
      '#type' => 'submit',
      '#value' => t("Get Relative Terms"),
      '#submit' => array('ontology_search_relatives_submit'),
      '#attributes' => array(
      'id' => 'getrel',
       )
     );

  if ((variable_get('term')!=null) && $similarTerms !=null) {
      show($form['get_relatives']);
      show($form['relationship']);
      show($form['search_term']);
      hide($form['No_Such_Term']);
     }
  else {
     hide($form['get_relatives']);
     hide($form['relationship']);
     hide($form['search_term']);
     if($similarTerms ==null && (variable_get('term')!=null))
        show($form['No_Such_Term']);
    }
  $form['clear'] = array(
    '#type' => 'submit',
    '#value' => t("Clear search"),
    '#submit' => array('ontology_search_clear_submit'),
    '#attributes' => array(
    'id' => 'clear',
     )
   );
 // If there are any cvterms, create html to display them
  $html = '';
  $html.="".$ab."";
  $found_list = unserialize(variable_get('FoundTerms'));
  $html="<table >";
  if($found_list !=null)
     $html .= "<tr  ><td >[Degree] </td><td>Name</td><td>Accession</td><td>Description</td><td>Ontology Name</td></tr>" ;
  foreach ($found_list as $cvterm) {
     $x=$cvterm['name'];
     drupal_add_js(drupal_get_path('module', 'ontology_search').'/theme/js/ontology_search_scripts.js');
     $x="".$cvterm['name']."";
     $html .= "<tr class=dom id='$x' style='cursor:pointer' ><td  >[". $cvterm['degree'] . "] </td><td >" . $cvterm['name']."</td><td>".$cvterm['accession']."</td><td>".$cvterm['def']."</td><td>"."[".$OntologiesLoaded[$cvterm['db_id']]."]</td></tr>" ;
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
function ontology_search_form_validate(&$form, &$form_state) {
  
  if (empty($form_state['values']['searchTerm'])) {
     form_set_error('searchTerm','search term cannot be empty'); 
  }
  else {
    // keep the search term for future reference
     variable_set('term', $form_state['values']['searchTerm']);
  }
  variable_set('Ontology_Selected', $form_state['values']['Ontologies_Avaliable']);
  variable_set('target_cvterm', $form_state['values']['search_term']);
}

/**
 * ontology_search_form_submit()
 *
 * Implements the initial term search submission.
 *
 * @ingroup ontology_search
 */
function ontology_search_form_submit($form, $form_state) {
  $term = $form_state['values']['searchTerm'];
  $vset = variable_get('Ontologies_Available');
  $OntologiesLoaded = get_Names_Ontologies_Available($vset);
  variable_del('FoundTerms');
  $filter_for_a_specific_ont =  $form_state['values']['Ontologies_Avaliable'] ;
  $similarTerms = ontology_search_setSimilarTerms($term, $OntologiesLoaded,$filter_for_a_specific_ont);
  variable_del('similarTerms');
  variable_set('similarTerms',$similarTerms);
  $form_state['values']['item'] = 'newvalue';
}


/**
 * ontology_search_relatives_submit()
 *
 * Implements the relatives search submission.
 *
 * @ingroup ontology_search
 */
function ontology_search_relatives_submit($form, $form_state) {
  $found_terms = false;  // false until/unless matching terms are found
  if (isset($form_state['values']['search_term']) && $form_state['values']['search_term'] != '') {
  $cvterm = $form_state['values']['search_term'];
  $relationship = $form_state['values']['relationship'];
  switch ($relationship) {
    case 'getParent':
      $found_list = ontology_search_getParent($cvterm);
      break;

    case 'getChildren':
      $found_list= ontology_search_getChild($cvterm);
      break; 

    case 'getSibilings':
      $found_list = ontology_search_getSibilings($cvterm);
      break; 
      
    }//switch
  }
  variable_set('Relative_Selected',$form_state['values']['relationship']);
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
  variable_del('Relative_Selected');
  variable_del('term');
  variable_del('similarTerms');
}


