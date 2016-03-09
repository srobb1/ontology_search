<?php
function ontology_search_form() {
    $form = array();

$form['some_text'] = array(
  '#markup' => '<p>ONTOLOGY SEARCH</p>'
);

$form['some_text2'] = array(
  '#markup' => '<p>Please type a word or phrase, then select a specific term to search for:</p>'
);

$form['name']=array(
        '#type'=>'textfield',
        '#title'=>t('Enter your search term here'),
        '#description'=>t('Your search term'),

      );
  

    $form['submit']=array(

        '#type'=>'submit',
        '#value'=>t('Search')
      );
  

 


    $similarTerms=  $_SESSION['similarTerms'];    
    $form['search_term'] = array(
    '#type' => 'select',
    '#title' => t('Select the specific term of your interest:'),
    '#options' => $similarTerms,//variable_get('mydata'),
    '#default_value' => '',//$defaults['max_target_seqs'],
    '#description' => t('which specific term are you looking for'),
      );

    $OntologiesLoaded=null;
   $vset=variable_get('Ontologies_Available');
    $OntologiesLoaded=$_SESSION['final'];
   $OntologiesLoaded=  get_Names_Ontologies_Available($vset);
   // drupal_set_message('<pre>'. print_r($vset, TRUE) .'</pre>');
    
    $form['Ontologies_Avaliable'] = array(
    '#type' => 'select',
    '#title' => t('ontologies available:'),
    '#options' => $OntologiesLoaded,//variable_get('mydata'),
    '#default_value' => '',//$defaults['max_target_seqs'],
    '#description' => t('You can further be specific about the ontology you want to search from'),
      );
 
    $relative= array('getParent','getChildren','getSibilings');
     $form['ALG']['GParam']['GW'] = array(
    '#type' => 'select',
    '#title' => t('Which Relatives:'),
    '#options' => $relative,//variable_get('mydata'),
    '#default_value' => '',//$defaults['max_target_seqs'],
    '#description' => t('Which relative of the selected term do you want'),
      );
    
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


    return $form;
}
// the main form used 


function ontology_search_form_validate($form, $form_state) {
  //drupal_set_message($form_state['values']['name']);
   drupal_set_message("I am here 121212");
   //drupal_set_message($name);
 // print_r($form_state['values']['#rows']['people']);
   $prefix = $form_state['values']['name'];
  // drupal_set_message("I am here 1");
   drupal_set_message($prefix);
  if(empty($form_state['values']['name']))
   {
     form_set_error('name','Name cannot be empty'); 
   }
}
//validate the form here



function ontology_search_form_submit($form, $form_state) {

 
  $prefix =$form_state['values']['name'];
  drupal_set_message("I am here finally");
  print_r($prefix);
  drupal_set_message($prefix);
  ontology_search_setSimilarTerms($prefix);
  $vset=variable_get('Ontologies_Available');
  get_Names_Ontologies_Available($vset);

 // $vset=variable_get('Ontologies_Available');
  //drupal_set_message($vset);
 // drupal_set_message('<pre>'. print_r($vset, TRUE) .'</pre>');

  // some thing missing here I have to debug
  //dont forget
  //ontology_search_setOntologies();
  // this function is called to set the session variable to similar terms
  //which is has beed used to put terms in the drop down 

}

