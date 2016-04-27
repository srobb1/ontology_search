(function($) {
  $(document).ready(function() {
 // $("#name_field").css('width', 240,'float','left');
 // $("#ont_avail").css({'float','left', position:'relative'});
 // $("#ont_avail").css({top: 120 , left: 300, position:'absolute'});
 // $('label[for="edit-ontologies-avaliable"]').css({top: 80, left: 320, position:'absolute'});
  
  
 // $("#sub").css({top: 100, left: 600, position:'absolute'});
  $(".dom").hover(function(){
       $(this).css('border', '2px solid black');
 $(this).mouseout(function(){
    $(this).css('border', '1px solid black');
});
       
}); 
    $('.dom').click(function() {
   
   $("#name_field").val(this.id);
     
   $("#ontology-search-form").submit();

 

    });
  });//ready
})(jQuery);