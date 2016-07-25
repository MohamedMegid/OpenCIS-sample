Drupal.behaviors.cis_system = function() {

  //$('#edit-field-service-report-job-nid-nid').attr('disabled','disabled');

  var get_emps_chbx = $('input[id=edit-field-job-employees-value]').is(':checked');
  if(get_emps_chbx == false) {
    $.get(Drupal.settings.basePath +'get_employees/equipment/'+ equipment_nid, null, function(response) { 
      //$('#edit-field-service-report-assigned-em-nid-nid').html(response);
    });
  }// end if

  $('#edit-field-job-employees-value').change(function(){
    var get_emps_chbx = $('input[id=edit-field-job-employees-value]').is(':checked');
    if(get_emps_chbx == false) {
      $.get(Drupal.settings.basePath +'get_employees/equipment/'+ 1, null, function(response) { 
        $('#edit-field-service-report-assigned-em-nid-nid').html(response);
      });
    }// end if
    else {
      $.get(Drupal.settings.basePath +'get_employees/equipment/'+ equipment_nid, null, function(response) { 
        $('#edit-field-service-report-assigned-em-nid-nid').html(response);
      });
    }
  });
}
