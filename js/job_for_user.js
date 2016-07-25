$(document).ready(function() {
  if ($(".hidden-tablet").css( "display" ) == 'block' && $(".hidden-phone").css("display") == 'block') {
    $('.page-container').addClass('sidebar-closed');
  }
  var job_id = $('#edit-title').val();
  var display_title = '<b><p>  Call Id : ' + job_id  + '</p></b>';   
  $('#edit-title-wrapper').after(display_title);
  $("#edit-title-wrapper").hide();
});
