<?php


class TermNode
{
    public $objectId;
    public $pathLength;
    public $relativeType;
    public $name;
    
    public $next;
    function __construct($objectId,$pathLength,$type)
     {
        $this->objectId = $objectId;
        $this->pathLength = $pathLength;
        $this->relativeType = $type;
        $this->name= NULL;
        $this->next = NULL;
     }
    function setName($nam)
     {
      $this->name = $nam;
     }
    function getName()
     {
      return $this->name;
     }
    function readNode()
     {
        return $this->objectId;
     }
     function getPathlength()
     {
        return $this->pathLength;
     }
    function getType()
     {
        return $this->relativeType;
     }
}


// This Acts as a Node  for the linkedlist used in all the below functions 

function Ontology_Search_Get_Relatives($form, $form_state)
{
   //$similarTerms=$_SESSION['similarTerms'];
if($form_state['values']['GW']=='0')
   { 
      $parents=ontology_search_getParent($form_state['values']['search_term']);
      printALL($parents);
  
   }
elseif($form_state['values']['GW']=='1')
   {
      $children=ontology_search_getChild($form_state['values']['search_term']);
      printALL($children);
   }
elseif($form_state['values']['GW']=='2')
   {
      $sibilings=ontology_search_getSibilings($form_state['values']['search_term']);
      printALL($sibilings);
   }

}

//Controller to select which Relative you want to search for 

function ontology_search_getParent($id)
{ 
   $prefix =$form_state['values']['name'];
   $count=0;
    if ($GLOBALS["chado_is_installed"]) 
     {
       if (!function_exists('chado_query')) {
           drupal_set_message("problem with chado_query");
         }
       $sql =  "SELECT object_id,type_id,pathdistance FROM cvtermpath WHERE subject_id = :subject_id ORDER BY pathdistance ASC";
       $results = db_query($sql, array(':subject_id' => $id));
       $sta= new \SplStack();
       foreach ($results as $record) {
          $count++;
          $n= new TermNode($record->object_id,$record->pathdistance,$record->type_id);
          $sta->push($n);
         }
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
       while($temp!=null)
         {
           $cv=$temp->readNode();
           $pl=$temp->getPathlength();
           $sql= "SELECT name FROM cvterm WHERE cvterm_id = :cvterm_id";
           $resu = db_query($sql, array(':cvterm_id' => $temp->readNode()));
           foreach ($resu as $record)
             {
               $temp->setName($record->name);
             }
           $temp=$temp->next;

         }
      return $head;
    }
}

// this function is used to get the parents !! 

function ontology_search_getSibilings($id)
{
  $prefix =$form_state['values']['name'];
  $count=0;
  if ($GLOBALS["chado_is_installed"]) {
     if (!function_exists('chado_query')) {
       drupal_set_message("chado_query doesnt exists");
        }
       $sql = "SELECT object_id,type_id,pathdistance FROM cvtermpath WHERE subject_id = :subject_id ORDER BY pathdistance ASC";
     $par1 = db_query($sql, array(':subject_id' => $id));
     if($par1==null)
       {
         drupal_set_message("no children");
       }
     $sta= new \SplStack();
     foreach ($par1 as $record) {
         $n= new TermNode($record->object_id,$record->pathdistance,$record->type_id);
         $sta->push($n);
       }
     $final=NULL;   
     $out=$sta->pop();
     while(!$sta->isEmpty())
      { 
       $pl = $out->getPathlength();
       $nv = $out->readNode();
       if($pl==1)
        {
          $va=$out->readNode();
          $chi=ontology_search_getChildrenatlevel1($va);
          if($final==NULL)
           {
            $final=$chi;
           }
          else
          {
            $temp=$final;
            while($temp->next!=NULL)
             {
              $temp = $temp->next;

             }
            $temp->next=$chi;

          }
           $out=$sta->pop();
        }
       else
        {
          $out=$sta->pop();
        } 
       }
       return $final;
    }
 }
// first function for sibilings gets the first parent

function ontology_search_getChildrenatlevel1($id)
{
  $sql =  "SELECT subject_id,type_id,pathdistance FROM cvtermpath WHERE object_id = :object_id ";
  $sib1 = db_query($sql, array(':object_id' => $id));
  if (!$sib1)
    {
     echo "An error to get the sibilings at level 1.\n";
     exit;
    }
  $stac= new \SplStack();
  foreach ($sib1 as $record) {
    if($record->pathdistance==1)
      {
        $n= new TermNode($record->subject_id,$record->pathdistance,$record->type_id);
        $stac->push($n);
      }
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

  while($temp != null)
   { 
     $cv=$temp->readNode();
     $cvp =$temp->getPathlength();
     $sql =  "SELECT name FROM cvterm WHERE cvterm_id = :cvterm_id ";
     $name = db_query($sql, array(':cvterm_id' => $cv));
     if (!$name) 
       {
         echo "name fetch error.\n";
         exit;
       }
      foreach ($name as $record) {
         $temp->setName($record->name);
       }
      $temp=$temp->next;
     }
   
   return $head;
}

//thi is second part of getSibiling 

function ontology_search_getChild($id)
{
  $prefix =$form_state['values']['name'];
  $count=0;
  if ($GLOBALS["chado_is_installed"]) {
    if (!function_exists('chado_query')) {
        drupal_set_message("chado_query doesnt exists");
      }
    $sql =  "SELECT subject_id,type_id,pathdistance FROM cvtermpath WHERE object_id = :object_id ORDER BY pathdistance DESC";
    $child = db_query($sql, array(':object_id' => $id));
    if($child==null){
      drupal_set_message("no children");
      }
    $sta= new \SplStack();
    foreach ($child as $record) {
        $n= new TermNode($record->subject_id,$record->pathdistance,$record->type_id);
        $sta->push($n);
      }
    
    if(!$sta->isEmpty())
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
    while($temp!=null)
      {
         $cv=$temp->readNode();
         $pl=$temp->getPathlength();
         $sql= "SELECT name FROM cvterm WHERE cvterm_id = :cvterm_id";
         $resu = db_query($sql, array(':cvterm_id' => $temp->readNode()));
         foreach ($resu as $record)
           {
            $temp->setName($record->name);
           }
         $temp=$temp->next;

      }
   }
  return $head;
}


// this is the function to get the children !! 

function ontology_search_setSimilarTerms($prefix)
{
  
  if($prefix != NULL)
  { 
  
  if ($GLOBALS["chado_is_installed"]) {
    if (function_exists('chado_query')) {
      drupal_set_message("exists");
     }
    $similarTerms = null;
 
    $results= db_query(
        'SELECT name,cvterm_id FROM cvterm WHERE name LIKE :name',
       array(':name' => '%'.db_like($prefix).'%')
      )->fetchAll();

    if($similarTerms==null)
     {
       foreach ($results as $r) {
       $similarTerms[$r->cvterm_id] = $r->name;
       }
     }
     //drupal_set_message('results: <pre>' . print_r($abc, true) . '</pre>');
     //just in case u want to check if all the similar terms are set in the session
     unset($_SESSION['similarTerms']);
     $_SESSION['similarTerms'] =$similarTerms;
    
    }
    

  }
}

// this function is used to populate the session with similar terms to the one which user entered


function printALL($head)
{ 
  $temp=$head;
  while($temp !=null)
   { 
     drupal_set_message($temp->getName());
     $temp=$temp->next; 
   }

}
// this is a function that prints names of all the elements that are passed through the linkedlist