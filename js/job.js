Drupal.behaviors.cis_system = function() {
  // handle responsible employee in job
    /*var url_param_arr = getUrlVars();
    alert(url_param_arr['q']);
    var equipment_nid = $('#edit-field-job-medical-equip-nid-nid').val();
    
    if ($('#edit-field-service-report-equip-nid-nid').val() !='') {
      equipment_nid = $('#edit-field-service-report-equip-nid-nid').val()
    }
    */

  var get_emps_chbx = $('input[id=edit-field-job-employees-value]').is(':checked');
  if(get_emps_chbx == false) {
    $.get(Drupal.settings.basePath +'get_employees/equipment/'+ equipment_nid, null, function(response) { 
      //$('#edit-field-job-assigned-emp-nid-nid').html(response);
      //$('#edit-field-service-report-assigned-em-nid-nid').html(response);
    });
  }// end if
  

  $('#edit-field-job-employees-value').change(function(){
    var get_emps_chbx = $('input[id=edit-field-job-employees-value]').is(':checked');
    if(get_emps_chbx == false) {
      $.get(Drupal.settings.basePath +'get_employees/equipment/'+ 1, null, function(response) { 
        $('#edit-field-job-assigned-emp-nid-nid').html(response);
        //$('#edit-field-service-report-assigned-em-nid-nid').html(response);
      });
    }// end if
    else {
      $.get(Drupal.settings.basePath +'get_employees/equipment/'+ equipment_nid, null, function(response) { 
        $('#edit-field-job-assigned-emp-nid-nid').html(response);
        //$('#edit-field-service-report-assigned-em-nid-nid').html(response);
      });
    }
  });
 

}
