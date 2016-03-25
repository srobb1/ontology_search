<?php
/**
 * @file
 * This file contains the functions used for administration of the module
 *
 */
function ontology_search_admin_examples_listing() {
  $output = '';

  $output = 'Some explanation about the Ontology Search module goes here';
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
    if (function_exists('chado_query')) {
//drupal_set_message("exists");
    }
    
    $results = db_query('SELECT name,cv_id FROM cv')->fetchAll();
    foreach ($results as $r) {
      //drupal_set_message($r->cv_id);
      $query='SELECT COUNT(cv_id) FROM cvterm where cv_id= :cv_id';
      $numberOfcvTerms=db_query($query, array(':cv_id' => $r->cv_id))->fetchField();
      //drupal_set_message($numberOfcvTerms);
      //var_dump($numberOfcvTerms);
      if($numberOfcvTerms>2)
       { 
        $NameCount="$r->name\t($numberOfcvTerms)";
        // drupal_set_message($NameCount);
       $ontologiesAvailable[$r->cv_id] = $NameCount;
       }

    }
  }
  
  // Get which ontologies have been selected, if any and set them as defaults.
  $defaults = array();
  if ($pids = variable_get('Ontologies_Available')) {
//drupal_set_message('pids: <pre>'. print_r($pids, TRUE) .'</pre>');
    foreach ($pids as $pid) {
      array_push($defaults, $pid);
    }
  }

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
 */
function ontology_search_admin_validate($form, &$form_state) {
	$pids = array_filter($form_state['values']['ontologies']);
//drupal_set_message('PIDs: <pre>'. print_r($pids, TRUE) .'</pre>');
//dr_set_message("breakpoint");
  
//EKSC: are these the ontologies that have been selected or all that are available?
//      Be sure that variable names are meaningful, correct, and accurate.

  variable_set('Ontologies_Available', $pids);
}

