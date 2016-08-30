<?php
/**
 * @file
 * This file contains the functions used for administration of the module
 *
 */
function ontology_search_admin_examples_listing() {
  $output = '';

  $output = 'Module for hierarchical searching of ontologies.';
  $output .= "<br><br>Click the 'Settings' tab to set up this module.";

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup ontology_search
 */
function ontology_search_admin() {
  $form = array();

  if ($GLOBALS["chado_is_installed"]) {
    if (!function_exists('chado_query')) {
        drupal_set_message(" chado_query does not exists");
        return null;
    }
    
    $results = db_query('SELECT name,cv_id FROM cv')->fetchAll();
    foreach ($results as $r) {
      $query='SELECT COUNT(cv_id) FROM cvterm where cv_id= :cv_id';
      $numberOfcvTerms=db_query($query, array(':cv_id' => $r->cv_id))->fetchField();
      
      if($numberOfcvTerms>2)
       { 
        $NameCount="$r->name\t($numberOfcvTerms)";
        $ontologiesAvailable[$r->cv_id] = $NameCount;
       }

    }
   
  }
  else
  {
    drupal_set_message(" chado not installed");
    return null;
  }
  
  // Get which ontologies have been selected, if any and set them as defaults.
  $defaults = array();
  if ($pids = variable_get('Ontologies_Available')) {
    foreach ($pids as $pid) {
      array_push($defaults, $pid);
    }
  }
  // to sort the available ontologies alphabetically
  
asort($ontologiesAvailable,SORT_FLAG_CASE | SORT_NATURAL);
  // the drupal checkboxes form field definition
  $form['ontologies'] = array(
    '#title' => t('Avaialable ontologies (Term count)'),
    '#type' => 'checkboxes',
    '#description' => t('Select the ontologies you would like users to search.'),
    '#options' => $ontologiesAvailable,
    '#default_value' => $defaults,
  );

  // Sets up a form to save information automatically.
  return system_settings_form($form);
}



/**
 *
 * @ingroup ontology_search 
 * Ontologies_Available are the Ontologies that are available for the User Selected by the Admin.
 */
function ontology_search_admin_validate($form, &$form_state) {
	$pids = array_filter($form_state['values']['ontologies']);
  variable_set('Ontologies_Available', $pids);
}

