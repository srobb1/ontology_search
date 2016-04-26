(function($) {
  $(document).ready(function() {
  $(".dom").hover(function(){
       $(this).css('border', '2px solid black');
 $(this).mouseout(function(){
    $(this).css('border', '1px solid black');
});
       
}); 
    $('.dom').click(function() {
    ///console.log($("#damn").html());
   // alert();
   $("#name_field").val(this.id);
      //$(".form-item form-type-select form-item-search-term").hide();
      
      //$(".form-item form-type-select form-item-relationship").hide();
      //alert();
  // alert($("#name_field").val());
   $("#ontology-search-form").submit();

   // $("#edit-submit").submit(function(e) {
//       alert('form submitted');
//      // alert($('form#my-form #edit-name').val());
//     });
   //alert($("#name_field").val());
    
   // var x='<?php echo $_SESSION['term'] ?>';
    //alert(x);
    //alert(form_state['values']['name']);
      var species = $('#otherspecies_species').find(":selected").val();
      var term = $.trim($("#otherspecies_term").val());
      if (!term || term == '') {
        return;
      }

      if (species == 'soybean') {
        window.location.href = "http://soybase.org//search/index.php?search=true&qtl="+term;
      }
    });
  });//ready
})(jQuery);