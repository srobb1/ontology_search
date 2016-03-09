<?php

function ontology_search_admin_form($form, $form_state)
{



if ($GLOBALS["chado_is_installed"]) {
    if (function_exists('chado_query')) {
      drupal_set_message("exists");
     }
    $ontologiesAvailable = null;
 
    $results= db_query(
        'SELECT name,cv_id FROM cv' 
      )->fetchAll();

    if($ontologiesAvailable==null)
     {
       foreach ($results as $r) {
       $ontologiesAvailable[$r->cv_id] = $r->name;
       }
     }
 }
    //  //drupal_set_message('results: <pre>' . print_r($abc, true) . '</pre>');
    //  //just in case u want to check if all the similar terms are set in the session
    //  unset($_SESSION['similarTerms']);
    //  $_SESSION['similarTerms'] =$similarTerms;
    
    // }

$toppings = array(
  'pepperoni' => t('Pepperoni'),
  'black_olives' => t('Black olives'), 
  'veggies' => t('Veggies')
);

# the drupal checkboxes form field definition
$form['pizza'] = array(
  '#title' => t('Pizza Topping'),
  '#type' => 'checkboxes',
  '#description' => t('Select the pizza toppings you would like.'),
  '#options' => $ontologiesAvailable,
);



  //return system_settings_form($form);

 $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save Configuration'
  );

  return $form;


}



function ontology_search_admin_form_validate($form,&$form_state)
{    //-drupal_set_message("i am here hoping something shows up ");

   
     
	$pids = array_filter($form_state['values']['pizza']);
    // drupal_set_message($pids);
     drupal_set_message('<pre>'. print_r($pids, TRUE) .'</pre>');

     //$ = $form_state['values']['pizza'];
     variable_set('Ontologies_Available', $pids);
     //print_r($pids);
	// var_dump($form_state['values']['pizza']);
	// print_r($form_state['values']['pizza']);
	// drupal_set_message($form_state['values']['pizza']);

}

function ontology_search_admin_form_submit($form,&$form_state)
{
  drupal_set_message("I am here I Have set the values I think");


}





