<?php
function ontology_search_form() {
    $form = array();


    $form['name']=array(
        '#type'=>'textfield',
        '#title'=>t('Enter your search term'),
        '#description'=>t('Your search term')
      );
  

    $form['submit']=array(
        '#type'=>'submit',
        '#value'=>t('Search')
      );
      
    $similarTerms=  $_SESSION['similarTerms'];    
    $form['search_term'] = array(
    '#type' => 'select',
    '#title' => t('Select the term that you were looking for:'),
    '#options' => $similarTerms,//variable_get('mydata'),
    '#default_value' => '',//$defaults['max_target_seqs'],
    '#description' => t('what term are you searching for'),
      );
 
    $relative= array('getParent','getChildren','getSibilings');
    $form['ALG']['GParam']['GW'] = array(
    '#type' => 'select',
    '#title' => t('get What:'),
    '#options' => $relative,//variable_get('mydata'),
    '#default_value' => '',//$defaults['max_target_seqs'],
    '#description' => t('Which relative of the selected term do you want'),
      );
    
    $form['query']['example_sequence'] = array(
    '#type' => 'checkbox',
    '#title' => t('get the Relatives selected for the search term'),
    '#prefix' => '<span style="float: left;">',
    '#suffix' => '</span>',
    '#ajax' => array(
      'callback' => 'Ontology_Search_Get_Relatives',
      'wrapper'  => 'fasta-textarea',
      'method'   => 'replace',
      'effect'   => 'fade',
          ),
      );


    return $form;
}
// the main form used 


function ontology_search_form_validate($form, $form_state) {

  if(empty($form_state['values']['name']))
   {
     form_set_error('name','Name cannot be empty');
   }
}
//validate the form here



function ontology_search_form_submit($form, $form_state) {

 
  $prefix =$form_state['values']['name'];
  ontology_search_setSimilarTerms($prefix);
  // this function is called to set the session variable to similar terms
  //which is has beed used to put terms in the drop down 

}

