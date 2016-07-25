Drupal.behaviors.cis_system = function() {
  $(document).ready(function() {
    // close the side bar after loading the page
    var current_language_path = "";
    if(window.location.href.indexOf("ar/") >= 0) {
      current_language_path = "ar/";
    }
    else {
      current_language_path = "";
    }
    if ($(".hidden-tablet").css( "display" ) == 'block' && $(".hidden-phone").css("display") == 'block') {
      $('.page-container').addClass('sidebar-closed');
    }
    close_lookup_esc_btn();
    //equipment_box_change();
    $('#edit-field-quotation-equ-op-type-value').attr('disabled', 'disabled');
    $('#edit-field-wrk-order-type-value').attr('disabled', 'disabled');
    $('#edit-field-quotation-working-hours-0-value').attr('disabled', 'disabled');
    $('#edit-field-quotation-traveling-hours-0-value').attr('disabled', 'disabled');
    $('.last_price').hide();
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // fill warranty months percentage automatic
    $('#edit-field-equipment-warrantymon-cost-0-value').blur(function(event) {
      var price = $('#edit-field-equipment-price-0-amount').val();
      var equipment_price = price.replace(/\,/g, '');
      var warranty_months_cost = $('#edit-field-equipment-warrantymon-cost-0-value').val();
      var warranty_months_percentage = 100 / (equipment_price / warranty_months_cost);
      if (price){
        $('#edit-field-equipment-warrantymon-perc-0-value').val(warranty_months_percentage.toPrecision(4));
      }
    });
    // fill extended months percentage automatic
    $('#edit-field-equipment-extendedmon-cost-0-value').blur(function(event){
      var price = $('#edit-field-equipment-price-0-amount').val();
      var equipment_price = price.replace(/\,/g, '');
      var extended_months_cost = $('#edit-field-equipment-extendedmon-cost-0-value').val();      
      var extended_warranty_months_percentag = (equipment_price / extended_months_cost);
      var extended_warranty_months_percentage = 100 / extended_warranty_months_percentag;
      if (price){
        $('#edit-field-equipment-extendedmon-perc-0-value').val(extended_warranty_months_percentage.toPrecision(4));
      }
    });

    // fill warranty cost automatic
    $('#edit-field-equipment-warrantymon-perc-0-value').blur(function(event){
      var price = $('#edit-field-equipment-price-0-amount').val();
      var equipment_price = price.replace(/\,/g, '');
      var warranty_percentage = $('#edit-field-equipment-warrantymon-perc-0-value').val();
      var warranty_cost = (equipment_price * warranty_percentage) / 100;
      if (price){
        $('#edit-field-equipment-warrantymon-cost-0-value').val(warranty_cost);
      }
    });
    // fill extended cost automatic
    $('#edit-field-equipment-extendedmon-perc-0-value').blur(function(event){
      var price = $('#edit-field-equipment-price-0-amount').val();
      var equipment_price = price.replace(/\,/g, '');
      var extended_percentage = $('#edit-field-equipment-extendedmon-perc-0-value').val();
      var extended_cost = (equipment_price * extended_percentage) / 100;
      if (price){
        $('#edit-field-equipment-extendedmon-cost-0-value').val(extended_cost);
      }
    });
//////////////////////////////FILL ASSIGNED EMPLOYEE AUTOMATIC @ MEDICAL EQUIPMENT///////////////////////////////////////
    $('#edit-field-equipment-client-nid-nid').blur(function(event) {
      var client = $('#edit-field-equipment-client-nid-nid').val();
      var split = client.split(":");
      var client_nid = split[1];
      client_nid = client_nid.substr(0,client_nid.length-1);
      if (client_nid){
        $.get(Drupal.settings.basePath +'fill/assigned/employee/auto/'+client_nid+'',null, function(response) {
          $('#edit-field-equipment-assigned-user-uid-uid').val(response);
        });
      }
      else if (!client_nid){
        $('#edit-field-equipment-assigned-user-uid-uid').val('');
      }
    });

////////////////////// CHECKBOX TO START THE CONTRACT AS A QUOTATION ///////////////////////////////////////
/*
    $('#edit-field-mcontracts-as-emp-contract-uid-uid-wrapper').hide();
    $('#edit-field-mcontracts-as-emp-quotatio-uid-uid-wrapper').hide();
    $('#edit-field-mcontracts-type-config-value-wrapper').hide();
    $('#edit-field-mcontracts-begin-as-quotat-value').change(function(event) {
      if(this.checked) {
        $('#edit-field-mcontracts-type-config-value').val(2);
        $('#edit-field-mcontracts-as-emp-quotatio-uid-uid-wrapper').show();
        $('#edit-field-mcontracts-type-config-value-wrapper').show();
      }
      else{
        $('#edit-field-mcontracts-as-emp-contract-uid-uid-wrapper').hide();
        $('#edit-field-mcontracts-as-emp-quotatio-uid-uid-wrapper').hide();
        $('#edit-field-mcontracts-type-config-value-wrapper').hide();
        $('#edit-field-mcontracts-type-config-value').val('');
      }
    });
    $('#edit-field-mcontracts-type-config-value').change(function(event){
      var contract_type = $('#edit-field-mcontracts-type-config-value').val();
      if (contract_type == 1){
        $('#edit-field-mcontracts-as-emp-contract-uid-uid-wrapper').show();
        $('#edit-field-mcontracts-as-emp-quotatio-uid-uid-wrapper').hide();
      }
      else if (contract_type == 2){
        $('#edit-field-mcontracts-as-emp-quotatio-uid-uid-wrapper').show();
        $('#edit-field-mcontracts-as-emp-contract-uid-uid-wrapper').hide();
      }
      else {
        $('#edit-field-mcontracts-as-emp-contract-uid-uid-wrapper').hide();
        $('#edit-field-mcontracts-as-emp-quotatio-uid-uid-wrapper').hide();
      }
    });
*/

//////////////////////////////////// HANDLE ADD MORE VALUES TO CALCULATE TOTAL @ TARGET CONTRACTS FORM/////////////////////////////////////////
    var x;
    for(x = 0; x <= 5; x++){
      $('#edit-group-target-group-of-fields-'+x+'-field-target-price-value').change(function(event) {
        change_total();
      });
      $('#edit-group-target-group-of-fields-'+x+'-field-target-count-value').change(function(event) {
        change_total();
      });
    }    
/********************** CHECKALL CHECKBOXES @ Search PPM & CALIBRATION Entries *****************************/
    $('#checkall').change(function(event) {
        if(this.checked) {
            $('.checkbox1').each(function() {
                this.checked = true;
            });
        }else{
            $('.checkbox1').each(function() {
                this.checked = false;
            });        
        }
    });
  //////////////////////////////////////////////////////////////////////////////////////////////////////////


    /****************************** AUTOCOMPLETE FIELDS RELATED TO SEARCH **********************************************/
   // autocomplete textfield to search client @ Medical equipment
    //$('.views-widget-filter-field_wrk_order_client_nid').hide();
    $('#edit-client').blur(function(){
      var my_field = $('#edit-client').val();
      if (my_field == '') {
        $('#edit-field-equipment-client-nid').val('All');
        return;
      }
      var split = my_field.split(":");
      var client = split[1];
      $('#edit-field-equipment-client-nid').val(client);
    });


    // autocomplete textfield to search client @ call & quotation
    $('.views-widget-filter-field_job_client_nid').hide();
    $('#edit-client').blur(function(){
      var my_field = $('#edit-client').val();
      if (my_field == '') {
        $('#edit-field-job-client-nid').val('All');
        return;
      }
      var split = my_field.split(":");
      var client = split[1];
      $('#edit-field-job-client-nid').val(client);
    });

    // autocomplete textfield to search client @ work order
    $('.views-widget-filter-field_wrk_order_client_nid').hide();
    $('#edit-client').blur(function(){
      var my_field = $('#edit-client').val();
      if (my_field == '') {
        $('#edit-field-wrk-order-client-nid').val('All');
        return;
      }
      var split = my_field.split(":");
      var client = split[1];
      $('#edit-field-wrk-order-client-nid').val(client);
    });

    // autocomplete textfield to search client @ ppm
    $('.views-widget-filter-field_equipment_client_nid').hide();
    $('#edit-client').blur(function(){
      var my_field = $('#edit-client').val();
      if (my_field == '') {
        $('#edit-field-equipment-client-nid').val('All');
        return;
      }
      var split = my_field.split(":");
      var client = split[1];
      $('#edit-field-equipment-client-nid').val(client);
    });


    var path = location.pathname.split("/");
    if (path[2] != 'store' && path[3] != 'parts'){
      // autocomplete textfield to search by equipment arabic name (transfer value to select list cck) => search equipments
      $('.views-widget-filter-field_equipment_name_value_many_to_one').hide();
      $('#edit-equipment-name').blur(function(){
        var equipment_name = $('#edit-equipment-name').val();
        $('#edit-field-equipment-name-value-many-to-one').val(equipment_name);
      });

      // autocomplete textfield to search by equipment english name (transfer value to select list cck) => search equipments
      $('.views-widget-filter-field_equ_name_english_value_many_to_one').hide();
      $('#edit-equipment-name-english').blur(function(){
        var equipment_name = $('#edit-equipment-name-english').val();
        $('#edit-field-equ-name-english-value-many-to-one').val(equipment_name);
      });
    }
    // autocomplete textfield to search by part english name (transfer value to select list cck) => search parts
    $('.views-widget-filter-field_part_data_part_name_e_value').hide();
    $('#edit-part-name').blur(function(){
      var part_name = $('#edit-part-name').val();
      $('#edit-field-part-data-part-name-e-value').val(part_name);
    });
    // autocomplete textfield to search by part number (transfer value to select list cck) => search parts
    //$('.views-widget-filter-title').hide();
    $('#edit-part-number').blur(function(){
      var part_number = $('#edit-part-number').val();
      $('#edit-title').val(part_number);
    });
    /******************************** END OF AUTOCOMPLETE FIELDS RELATED TO SEARCH *************************************/
    // show div for add new component to be fixed in the service report / work order
    $('#showDiv').click(function(){
      var clickit = $(this).is(':click');
      if(clickit == true){
        document.getElementById('add_new_component').style.display = "block";
      }
    });
    $('#close_form').click(function(){
      var press = $(this).is(':click');
      if(press == true){
        $('#add_new_component').hide();;
      }
    });
     // handle save new component in the service report / work order
    $('#save_new_component').click(function() {
      var clicked = $(this).is(':click');
      if(clicked == true){
        var str = $('#edit-field-service-report-equip-nid-nid').val();
        if (str){
          var split = str.split(":");
        }
        if (split){
          var equipment_nid = split[1].substring(0, split[1].length - 1);
        }
        var name_in_arabic = $('input#arabic_name').val();
        var name_in_english = $('input#english_name').val();
        if ( name_in_arabic == "" && name_in_english == ""){
          alert("fill the form out");
          return false;
        }
        else if ( name_in_arabic == null || name_in_arabic == ""){
          alert("Arabic name must be filled out");
          return false;
        }
        else if ( name_in_english == null || name_in_english == ""){
          alert("English name must be filled out");
          return false;
        }
        else{
          alert("your information is saved and updated in component to be fixed list");
        }
        // Ajax call to save english and arabic name and it returns he name to be appended
        $.get(Drupal.settings.basePath +'add/new/component/'+name_in_arabic+'/'+name_in_english+'/'+equipment_nid+'',null, function(response) {
          $('#edit-field-spare-component-value').append(response);
        });
       clearInputs();
      }
    });
    // function to clear inputs in add new component html form
    function clearInputs(){
      $('input#arabic_name').val('');
      $('input#english_name').val('');
    }
    // function to clear inputs in item data input
    function clearmyinputs(){
      $('input#add_arabic_name').val('');
      $('input#add_english_name').val('');
    }

    $('#close').click(function(){
      var press = $(this).is(':click');
      if(press == true){
        $('#add_new_equipment').hide();;
      }
    });

    // show div for add equipment name in english and arabic in item data input form
    $('#show_my_div').click(function(){
      var click = $(this).is(':click');
      if(click == true){
        document.getElementById('add_new_equipment').style.display = "block";
      }
    });

    // handle save new equipment name in english and arabic
    $('#save').click(function(){
      var press = $(this).is(':click');
      if(press == true){
        var name_in_arabic = $('input#add_arabic_name').val();
        var name_in_english = $('input#add_english_name').val();
        if ( name_in_arabic == "" && name_in_english == ""){
          alert("fill the form out");
          return false;
        }
        else if ( name_in_arabic == null || name_in_arabic == ""){
          alert("Arabic name must be filled out");
          return false;
        }
        else if ( name_in_english == null || name_in_english == ""){
          alert("English name must be filled out");
          return false;
        }
        else{
          alert("your information is saved and updated in Equipment english and arabic lists");
        }

        $.get(Drupal.settings.basePath +'save/new/equipment/'+name_in_arabic+'/'+name_in_english+'', null, function(response){
          $('#edit-field-equipment-name-value').append('<option>'+name_in_arabic+'</option>');
          $('#edit-field-equ-name-english-value').append('<option>'+name_in_english+'</option>');
        });

        clearmyinputs();
      } 
    });

    $('.vertical-tabs-list-group_user_info').parent().css('display', 'none'); 
    var code_item_text_box_id = 'edit-group-spare-parts-0-field-quotation-item-code-nid-nid' ;

    // show dialog for part catalog lookup to let the user choose from list of parts code 
    $('.lookup').click(function() {
      $('.lookup_part_catalog_popbox').show();    
      code_item_text_id = $(this).parent('').parent('').attr("id");
      code_item_text_id = code_item_text_id.replace('-wrapper', '');
      window.code_item_text_id = code_item_text_id;
      var lookup_html;
      $.get(Drupal.settings.basePath +current_language_path+'return_part_catalog_search_form/0/0/_', null, function(response) { 
        if (Drupal.jsEnabled) {
          lookup_html = '<div class="cover"></div><div class="lookup_part_catalog_popbox"><div class="lookup_search">Part Catalog lookup search</div>' + response + '</div>' ;
          $('#edit-group-spare-parts-group-spare-parts-add-more').after(lookup_html);
        }
      });
      $('.lookup_part_catalog_popbox').remove();
    });

    // show dialog for equipment lookup to let the user choose from list of equipment
    $('.lookup_equipment').click(function() {
      $('.lookup_equipment_popbox').show();   
      equipment_text_id = $(this).parent('').parent('').attr("id");
      equipment_text_id = equipment_text_id.replace('-wrapper', '');
      window.equipment_text_id = equipment_text_id;
      window.equipment_arr = new Array();
      var lookup_html;
      var inline_script = "<script>" + " equipment_box_change();" + "</script>";
      $.get(Drupal.settings.basePath + current_language_path+'return_equipment_search_form/0/0/0/_/_', null, function(response) { 
        if (Drupal.jsEnabled) {
          lookup_html = '<div class="cover"></div><div class="lookup_equipment_popbox"><div class="lookup_search">Equipment lookup search</div>' + response + inline_script +'</div>' ;
          $('.lookup_equipment').after(lookup_html);
        }
      });
    });

    // show dialog for equipment lookup to let the user choose from list of equipment
    $('.lookup_coding').click(function() {
      $('.lookup_model_popbox').hide();
      $('.lookup_model_popbox').show();
      var lookup_html;
      $.get(Drupal.settings.basePath +current_language_path+'return_coding_search_form/0/0/0/_', null, function(response) { 
        if (Drupal.jsEnabled) {
          lookup_html = '<div class="cover"></div><div class="lookup_model_popbox"><div class="lookup_search">Model lookup search</div>' + response + '</div>';
          $('.lookup_coding').after(lookup_html);
        }
      });
    });
   // hide div
    $('.hierarchical-select-wrapper-for-name-edit-field-equipment-status-tids').prev().css('display', 'none');
 
    // handle change event for part name english
    $('#edit-field-part-data-part-name-e-value').change(function(){
      var english_name_value = $('#edit-field-part-data-part-name-e-value').val();
      $('#edit-field-part-data-part-name-a-value').val(english_name_value);
    });
    // handle change event for part name arabic
    $('#edit-field-part-data-part-name-a-value').change(function(){
      var arabic_name_value = $('#edit-field-part-data-part-name-a-value').val();
      $('#edit-field-part-data-part-name-e-value').val(arabic_name_value);
    });
    // map the changed equipment name in english and arabic and vice versa
    $('#edit-field-equipment-name-value').change(function(){
      var arabic_name_value = $('#edit-field-equipment-name-value').val();
      $('#edit-field-equ-name-english-value').val(arabic_name_value);
    });

    // blur event for autocomplete name of equipment
    $('#edit-field-equ-name-english-0-value').blur(function(){
      var eng_name_value = $('#edit-field-equ-name-english-0-value').val();
      $.get(Drupal.settings.basePath +'get_equipment_name/ar/' + eng_name_value, null, function(response) {
      	$('#edit-field-equipment-name-0-value').val(response);
      });
    });
    $('#edit-field-equipment-name-0-value').blur(function(){
      var arabic_name_value = $('#edit-field-equipment-name-0-value').val();
      $.get(Drupal.settings.basePath +'get_equipment_name/eng/' + arabic_name_value, null, function(response) {
        $('#edit-field-equ-name-english-0-value').val(response);
      });
    });
    $('#edit-field-equ-name-english-value').change(function(){
      var eng_name_value = $('#edit-field-equ-name-english-value').val();
      $('#edit-field-equipment-name-value').val(eng_name_value);
    });
    // handle on change event for department select box
    $('#edit-field-equipment-department-nid-nid').change(function(){
      var department_value = $('#edit-field-equipment-department-nid-nid').val();
      $.get(Drupal.settings.basePath +'get_sub_department/'+ department_value + '/form', null, function(response) { 
        $('#edit-field-equipment-sub-department-nid-nid').html(response);
      });
    });


         // handle on change event for department select box
    $('#edit-department').change(function(){
      var department_value = $('#edit-department').val();
      //alert (department_value);
      $.get(Drupal.settings.basePath +'get_sub_department/'+ department_value + '/form', null, function(response) {
        $('#edit-sub-department').html(response);
      });
    });
 
     // blur event for autocomplete brand of equipment
    $('#edit-field-type-l2-0-value').blur(function(){
      var model_value = $('#edit-field-type-l2-0-value').val();
      $.get(Drupal.settings.basePath +'get/equipment/brand/'+ model_value , null, function(response) {
        $('#edit-field-type-l1-0-value').val(response);
      });
    });
    //  handel the even the views exposed filter
    // handle on change event for department select box
  
    $('#edit-field-equipment-department-nid').change(function(){
      var department_value = $('#edit-field-equipment-department-nid').val();
      $.get(Drupal.settings.basePath +'get_sub_department/'+ department_value + '/view', null, function(response) { 
        $('#edit-field-equipment-sub-department-nid').html(response);
      });
    });

   // map the changed equipment name in english and arabic and vice versa
    $('#edit-field-equ-name-english-value').change(function(){
      var equipment_name_value = $('#edit-field-equ-name-english-value').val();
      $.get(Drupal.settings.basePath +'get_equipment_ppm_tasks/'+ equipment_name_value + '/form', null, function(response) { 
        $('#edit-field-ppm-tasks-retrieved-0-value').val(response);
      });
    }); 


  /***********load information of equipment information using on change event of autocomplete text box of coding****/
    $('#edit-field-equipment-coding-0-nid-nid').val;
    $('#edit-field-equipment-coding-0-nid-nid').blur(function() {
      // changed to blur cause google chrome is not handling the change event
      var text = $(this).val();
      if (text.search(']') > -1) {  // this means the user make the full autocomplete
        // now we can make the ajax call
        // ajax call to get information about equipment
        // equipment name, brand, model, make
        var code_txt = $('input[id=edit-field-equipment-coding-0-nid-nid]').val();
        var code_arr = code_txt.split(":");
        code_nid = code_arr[1].replace(']', '')
        $.get(Drupal.settings.basePath +'get_equipment_info/' + code_nid , null, equipmentDetails);
      }
    });
    var equipmentDetails = function(response) {
      var result = Drupal.parseJson(response);
      $('#edit-field-equipment-name-value').val(result.equipment_name);
      $('#edit-field-equ-name-english-value').val(result.equipment_name);
      $('#edit-field-type-l1-value').val(result.brand);
      $('#edit-field-type-l2-value').val(result.model);
      $('#edit-field-type-l2-value-wrapper').show();
      $('#edit-field-make-text-value').val(result.make);
      $('#edit-field-equipment-base-sup-comp-nid-nid').val(result.supplier);
    }



    // on change of ppm time period based on warranty
    $('#edit-field-equipment-ppm-months-value').change(function(){
      change_dates_of_ppm_based_on_warranty();
    });

    // on change of calibration time period based on warranty
    $('#edit-field-equipment-cal-months-value').change(function(){
      change_dates_of_cal_based_on_warranty(); 
    });

    // on change of operation date based on warranty
    $('#edit-field-equipment-operation-date-0-value-datepicker-popup-0-wrapper').change(function(){
      change_dates_of_ppm_based_on_warranty();
      change_dates_of_cal_based_on_warranty();
    });

    // on change of warranty months based on warranty
    $('#edit-field-equipment-warranty-months-value').change(function(){
      change_dates_of_ppm_based_on_warranty();
      change_dates_of_cal_based_on_warranty();
    });

    // on change of extended warranty months based on warranty
    $('#edit-field-equipment-ext-warranty-mon-value').change(function(){
      change_dates_of_ppm_based_on_warranty();
      change_dates_of_cal_based_on_warranty();
    });

  }); // end of document ready
}


function wrk_assigned_emp_checkbox() {
  var group = $('input[name=assigned_emp_check]:checked').val()
  if (group > 0) {
    $.get(Drupal.settings.basePath +'get_users_group'+'/' + group, null, function(response) { 
      $('#edit-field-service-report-assigned-em-uid-uid').html(response);  
    });
  }
  else 
     $.get(Drupal.settings.basePath +'get_users_group'+'/' + 0, null, function(response) { 
      $('#edit-field-job-assigned-user-uid-uid').html(response);  
    }); 
}

function assigned_emp_checkbox() {
  var group = $('input[name=assigned_emp_check]:checked').val()
  if (group > 0) {
    $.get(Drupal.settings.basePath +'get_users_group'+'/' + group, null, function(response) { 
      $('#edit-field-job-assigned-user-uid-uid').html(response);  
    });
  }
  else 
     $.get(Drupal.settings.basePath +'get_users_group'+'/' + 0, null, function(response) { 
      $('#edit-field-job-assigned-user-uid-uid').html(response);  
    }); 
}

function apply_selected_part_catalog() {
  $('.cover').css('display', 'none');
  var radio_code_value = $('input[name=code_select]:checked').val();
  window.code_item_text_id = '#' + window.code_item_text_id;
  $('#edit-field-equipment-coding-0-nid-nid' ).val(radio_code_value);
  var code_txt = $('input[id=edit-field-equipment-coding-0-nid-nid]').val();
  var code_arr = code_txt.split(':');
  var code_nid = code_arr[1].replace(']', '');
  $('#edit-field-type-l1-value').attr('value',828);
  $.get(Drupal.settings.basePath +'get_equipment_info/' + code_nid , null, function(response) {
    var result = Drupal.parseJson(response);
    $('#edit-field-equipment-name-value').val(result.equipment_name);
    $('#edit-field-equ-name-english-value').val(result.equipment_name);
    $('#edit-field-type-l1-value').val(result.brand);
    $('#edit-field-type-l2-value').val(result.model);
    $('#edit-field-type-l2-value-wrapper').show();
    $('#edit-field-make-text-value').val(result.make);
    $('#edit-field-equipment-base-sup-comp-nid-nid').val(result.supplier);
  });
  $('.lookup_model_popbox').remove();
}

function close_lookup_esc_btn(){
  $(document).keydown(function(e) {
    // ESCAPE key pressed
    if (e.keyCode == 27) {
      $('.cover').css('display', 'none');
      $('.lookup_part_catalog_popbox').remove();
      $('.lookup_equipment_popbox').remove();
      $('.lookup_model_popbox').remove();
    }
  });
}

function close_lookup () {
  $('.cover').css('display', 'none');
  $('.lookup_part_catalog_popbox').remove();
  $('.lookup_equipment_popbox').remove();
  $('.lookup_model_popbox').remove();
}
function apply_selected_spare_part() {
  $('.cover').css('display', 'none');
  $('.lookup_part_catalog_popbox').hide();
  var radio_code_value = $('input[name=code_select]:checked').val();
  window.code_item_text_id = '#' + window.code_item_text_id;
  $(window.code_item_text_id ).val(radio_code_value);
  var code_arr = window.code_item_text_id.split('-');
  window.code_count_num = code_arr[4];
  var radio_id = $('input[name=code_select]:checked').attr('id');
  var part_name = $('input[name=code_select]:checked').parent().next().next().html();
  $('#edit-group-spare-parts-'+ window.code_count_num  +'-field-quotation-part-name-value').val(part_name);
  $('#edit-group-spare-parts-'+ window.code_count_num  +'-field-wrk-order-part-name-value').val(part_name);
  $('#edit-group-spare-parts-'+ window.code_count_num  +'-field-importcode-part-name-value').val(part_name);
  $('#edit-group-spare-parts-'+ window.code_count_num  +'-field-job-part-name-value').val(part_name);
  $('#edit-group-spare-parts-'+ window.code_count_num  +'-field-invoice-part-name-value').val(part_name);
  $('#edit-group-spare-parts-'+ window.code_count_num +'-field-quotaion-items-count-value-wrapper > .description').html( ' Qty Available = <b>' +radio_id + '</b>');
  var part_no = $('#edit-group-spare-parts-'+ window.code_count_num  +'-field-quotation-item-code-nid-nid').val();
  var wo_type = $('#edit-field-wrk-order-type-value').val();
  if (wo_type){
    var part_name_wo = $('#edit-group-spare-parts-'+ window.code_count_num  +'-field-wrk-order-part-name-value').val();
  }
  if (part_no){
    var split = part_no.split(':');
    var part_nid = split[1].substring(0, split[1].length - 1);
  }
  var call = $('#edit-field-quotation-job-nid-nid').val();
  if (call){
    var split = call.split(":");
    var call_nid = split[1];
    call_nid = call_nid.substr(0,call_nid.length-1);
  }
  $.get(Drupal.settings.basePath +'get/last/part/price/wo/'+part_name_wo, null, function(response) {
    $('#edit-group-spare-parts-'+ window.code_count_num +'-field-wrk-order-unit-price-value-wrapper').children().next().next().children().show();
    $('#edit-group-spare-parts-'+ window.code_count_num +'-field-wrk-order-unit-price-value-wrapper').children().next().next().children().children().after(response);
  });
  $.get(Drupal.settings.basePath +'get/last/part/price/'+part_nid+ '/' + call_nid, null, function(response) {
    $('#edit-group-spare-parts-'+ window.code_count_num +'-field-quotation-unit-price-value').val(response);
  });
  $('.lookup_part_catalog_popbox').remove();
}
function search_and_get_result_to_lookup () {
  var inline_script = "<script>" + " equipment_box_change();" + "</script>";
  var sn = $('#edit-sn').val();
  var equipment_name = $('#edit-equipment-name').val();
  var brand = $('#edit-ffield-type-l1-value').val();
  var model = $('#edit-ffield-type-l2-value').val();
  var client = $('#edit-client').val();
  if (sn.length == 0) {
    sn = '_';
  }
  if (model.length == 0) {
    model = '0';
  }
  if (!client) {
    client = '_';
  }
  if (sn == '_' && brand==0 && model ==0 && equipment_name == 0 && client == '_') {
  }

  else {
    $.get(Drupal.settings.basePath +'return_equipment_search_form'+'/'+ equipment_name + '/' + brand + '/' + model + '/' + sn + '/' + client, null, function(response) {
      $('.lookup-table').html(response + inline_script);
    });
  }
}

var equipment_arr = new Array();
window.equipment_arr = equipment_arr;
function equipment_box_change() {
  //equipment_arr = new Array();
  var nid_val = 0;
  $('input:checkbox[name=equipment_selected]').change(function(){
    //nid_val = 
    if ($(this).attr('checked') == true) {
      nid_val = $(this).attr('id');
      console.log(nid_val);
      window.equipment_arr[nid_val] = '<tr><td>' + $(this).val() + '</td><td>' + $(this).attr('class')+ '</td></tr>';
    }
    else {
     // unset the value
    }
    
  });
}

function apply_selected_equipments() {
  $('.lookup_equipment_popbox').remove();   
  $('.cover').css('display', 'none');
  var selectedVariants = '';
  var table_html = '';
  table_html = table_html + '<tr><th>S.N</th><th>Equipment name</th></tr>';
  console.log(equipment_arr);
  $.each(window.equipment_arr, function( index, value ) {
    if (value !== undefined) {
      selectedVariants = selectedVariants + (index) + ',';
      table_html = table_html + value;
      console.log(value);
      console.log(table_html);
    }
    else {

    }
  });
  $('#edit-field-mcontracts-selected-eqs-0-value').val(selectedVariants);
  $('#selected_equipments').html('<table id ="tbl_selected">'+table_html+'</table>');
}

function apply_selected_equipments_with_assigned_employee() {
  $('.lookup_equipment_popbox').remove();
  $('.cover').css('display', 'none');
  var selectedVariants = '';
  var table_html = '';
  table_html = table_html + '<tr><th>S.N</th><th>Equipment name</th></tr>';
  console.log(equipment_arr);
  $.each(window.equipment_arr, function( index, value ) {
    if (value !== undefined) {
      selectedVariants = selectedVariants + (index) + ',';
      table_html = table_html + value;
      console.log(value);
      console.log(table_html);
    }
    else {

    }
  });
  $('#edit-field-mcontracts-selected-eqs-0-value').val(selectedVariants);
  $('#selected_equipments').html('<table id ="tbl_selected">'+table_html+'</table>');
  get_equipment_nid_from_lookup_mc();
}

function get_equipment_nid_from_lookup_mc(){
  var selected_equipment = $('#edit-field-mcontracts-selected-eqs-0-value').val();
  var split = selected_equipment.split(",");
  var equ_nid = split[0];
  if (equ_nid){
    $.get(Drupal.settings.basePath +'fill/assigned/employee/auto/mc/'+equ_nid+'',null, function(response) {
      $('#edit-field-mcontracts-assigned-user-uid-uid').val(response);
    });
  }
  else {
    $('#edit-field-mcontracts-assigned-user-uid-uid').val('');
  }
}
function change_total(){
  var i;
  for(i = 0; i <= 3; i++){
    var price = $('#edit-group-target-group-of-fields-'+i+'-field-target-price-value').val();
    var count = $('#edit-group-target-group-of-fields-'+i+'-field-target-count-value').val();
    var total = price * count;
    $('#edit-group-target-group-of-fields-'+i+'-field-target-total-value').val(total);
  }
}
function getParameterByName(name) {
  name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
  var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
  results = regex.exec(location.search);
  return results == null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}
/**
 * change dates of ppm based on warranty
 */
function change_dates_of_ppm_based_on_warranty(){
  var ppm_months = $('#edit-field-equipment-ppm-months-value').val();
  if (ppm_months == '')  {
    $('#ppms_item').html('');
    $('#edit-field-ppm-plan-dates-0-value-datepicker-popup-0').val('');
    return;
  }
  var ppm_period = 12 / ppm_months;
  var supply_date = $('#edit-field-equipment-date-0-value-datepicker-popup-0').val();
  var mymonth = ppm_months / 1 ;
  var ppms_html = '';
  var supply_date_arr = supply_date.split('/');
  var day = supply_date_arr[0];
  var month = supply_date_arr[1];
  var year = supply_date_arr[2];
  var myDate = new Date(month + "/" + day + "/" + year);
  var warranty_end_date = $('#edit-field-warranty-end-date-0-value-datepicker-popup-0').val();
  var operation_date = $('#edit-field-equipment-operation-date-0-value-datepicker-popup-0').val();
  var warranty_months = $('#edit-field-equipment-warranty-months-value').val();
  var my_ppm_months = $('#edit-field-equipment-ppm-months-value').val();
  var extended_warranty_months = $('#edit-field-equipment-ext-warranty-mon-value').val();
  var my_extended_warranty_months = extended_warranty_months / 1 ;
  var new_warranty_end_date = '';
  var operation_date_arr = operation_date.split('/');
  var operation_day = operation_date_arr[0];
  var operation_month = operation_date_arr[1];
  var operation_year = operation_date_arr[2];
  var my_warranty_months = warranty_months / 1 ;
  var my_operation_Date = new Date(operation_month + "/" + operation_day + "/" + operation_year);
  var my_new_operation_date = new Date(operation_month + "/" + operation_day + "/" + operation_year);
  if (operation_date && supply_date && !warranty_end_date){
    //var date_format = my_operation_Date.addMonths(my_warranty_months) ;
    var date_format = new Date(new Date(my_operation_Date).setMonth(my_operation_Date.getMonth()+my_warranty_months));
    warranty_end_date = date_format;
  }
  else if (operation_date && !supply_date){
    var date_format = new Date(new Date(my_operation_Date).setMonth(my_operation_Date.getMonth()+my_warranty_months));//my_operation_Date.addMonths(my_warranty_months) ;
    warranty_end_date = date_format;
  }
  else if (!operation_date && supply_date){
    warranty_months = parseInt(warranty_months) + 3;
    //var date_format = myDate.addMonths(warranty_months) ;
    var date_format = new Date(new Date(myDate).setMonth(myDate.getMonth()+warranty_months));
    warranty_end_date = date_format;
  }
  else if (operation_date && supply_date && warranty_end_date){
    //var date_format = my_operation_Date.addMonths(my_warranty_months) ;
    var date_format = new Date(new Date(my_operation_Date).setMonth(my_operation_Date.getMonth()+my_warranty_months));
    new_warranty_end_date = date_format;
      console.log("new warranty end date");
    if (new_warranty_end_date < warranty_end_date){
      warranty_end_date = new_warranty_end_date;
    }
    else {
      warranty_end_date = date_format;
    }
  }
  if (extended_warranty_months && warranty_end_date){
   console.log("warranty date " + warranty_end_date);
    var w_months = warranty_end_date.getMonth();
    var my_date_format = new Date(new Date(warranty_end_date).setMonth(w_months+my_extended_warranty_months));//warranty_end_date.addMonths(my_extended_warranty_months) ;
    warranty_end_date = my_date_format;
  }
  var i = 100;
  var dateString = my_new_operation_date;
  dateString = new Date(dateString);
  dateString = parseInt(dateString.getTime() / 1000);
  warranty_end_date = new Date(warranty_end_date);
  warranty_end_date = parseInt(warranty_end_date.getTime() / 1000);
  var path = location.pathname.split("/");
  if (path[2] == 'ar'){
    ppms_html = ppms_html + '<label>من فضلك اختر التاريخ الذي تريد بدء خطة الزيارات الدورية منه:</label>';
  }
  else{
    ppms_html = ppms_html + '<label>Please choose the first date that you want to start your PPM plan on:</label>';
  }
  while (dateString < warranty_end_date){
    my_new_operation_date =  new Date(new Date(my_new_operation_date).setMonth(my_new_operation_date.getMonth()+mymonth));//my_new_operation_date.addMonths(mymonth) ;
    operation_date = $.datepicker.formatDate('dd/m/yy', my_new_operation_date);

dateString =  $.datepicker.formatDate('m/dd/yy', my_new_operation_date);//$.format.date(my_new_operation_date, 'dd/M/yy'); //my_new_operation_date.toLocaleFormat('%m/%d/%Y');
    dateString = new Date(dateString);
    dateString = parseInt(dateString.getTime() / 1000);
    ppms_html = ppms_html + '<input type="radio" id="'+i+'" name="radio_buttons" value="'+ operation_date +'">'+operation_date+'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    i++;
  }
  $('#ppms_item').html(ppms_html);
  document.getElementById("100").checked = true;
  var first_radio = $('#100').val();
  $('#edit-field-ppm-plan-dates-0-value-datepicker-popup-0').val(first_radio);
  $('input[name="radio_buttons"]').change(function(){
    var radiovalue = $('input[name="radio_buttons"]:checked').val();
    $('#edit-field-ppm-plan-dates-0-value-datepicker-popup-0').val(radiovalue);
  }); 
}

/**
 * change dates of calibration based on warranty
 */
function change_dates_of_cal_based_on_warranty(){
  var calibration_months = $('#edit-field-equipment-cal-months-value').val();
  if (calibration_months =='') {
    $('#calibrations_item').html('');
    $('#edit-field-calibration-plan-dates-0-value-datepicker-popup-0').val('');
    return;
  }
  var calibration_period = 12 / calibration_months;
  var supply_date = $('#edit-field-equipment-date-0-value-datepicker-popup-0').val();
  var mymonth = calibration_months / 1 ;
  var calibrations_html = '';
  var supply_date_arr = supply_date.split('/');
  var day = supply_date_arr[0];
  var month = supply_date_arr[1];
  var year = supply_date_arr[2];
  var myDate = new Date(month + "/" + day + "/" + year);
  var warranty_end_date = $('#edit-field-warranty-end-date-0-value-datepicker-popup-0').val();
  var operation_date = $('#edit-field-equipment-operation-date-0-value-datepicker-popup-0').val();

  var warranty_months = $('#edit-field-equipment-warranty-months-value').val();
  var my_calibration_months = $('#edit-field-equipment-cal-months-value').val();
  var extended_warranty_months = $('#edit-field-equipment-ext-warranty-mon-value').val();
  var my_extended_warranty_months = extended_warranty_months / 1 ;
  var new_warranty_end_date = '';
  var operation_date_arr = operation_date.split('/');
  var operation_day = operation_date_arr[0];
  var operation_month = operation_date_arr[1];
  var operation_year = operation_date_arr[2];
  var my_warranty_months = warranty_months / 1 ;
  var my_operation_Date = new Date(operation_month + "/" + operation_day + "/" + operation_year);
  var my_new_operation_date = new Date(operation_month + "/" + operation_day + "/" + operation_year);
  if (operation_date && supply_date && !warranty_end_date){
    //var date_format = my_operation_Date.addMonths(my_warranty_months) ;
    var date_format = new Date(new Date(my_operation_Date).setMonth(my_operation_Date.getMonth()+my_warranty_months));
    warranty_end_date = date_format;
  }
  else if (operation_date && !supply_date){
    var date_format = new Date(new Date(my_operation_Date).setMonth(my_operation_Date.getMonth()+my_warranty_months));//my_operation_Date.addMonths(my_warranty_months) ;
    warranty_end_date = date_format;
}
  else if (!operation_date && supply_date){
    warranty_months = parseInt(warranty_months) + 3;
    //var date_format = myDate.addMonths(warranty_months) ;
    var date_format = new Date(new Date(myDate).setMonth(myDate.getMonth()+warranty_months));
    warranty_end_date = date_format;
  }
  else if (operation_date && supply_date && warranty_end_date){
    //var date_format = my_operation_Date.addMonths(my_warranty_months) ;
    var date_format = new Date(new Date(my_operation_Date).setMonth(my_operation_Date.getMonth()+my_warranty_months));
    new_warranty_end_date = date_format;
    if (new_warranty_end_date < warranty_end_date){
      warranty_end_date = new_warranty_end_date;
    }
    else{
      warranty_end_date = date_format;
    }
  }
  if (extended_warranty_months && warranty_end_date){
    var my_date_format = new Date(new Date(warranty_end_date).setMonth(warranty_end_date.getMonth()+my_extended_warranty_months));//warranty_end_date.addMonths(my_extended_warranty_months) ;
    warranty_end_date = my_date_format;
  }
  var i = 1;
  var dateString = my_new_operation_date;
  dateString = new Date(dateString);
  dateString = parseInt(dateString.getTime() / 1000);
  warranty_end_date = new Date(warranty_end_date);
  warranty_end_date = parseInt(warranty_end_date.getTime() / 1000);
  var path = location.pathname.split("/");
  if (path[2] == 'ar'){
    calibrations_html = calibrations_html +'<label>من فضلك اختر التاريخ الذي تريد بدء خطة المعايرات منه:</label>';
  }
  else{
    calibrations_html = calibrations_html +'<label>Please choose the first date that you want to start your Calibration plan on:</label>';
  }
  while (dateString < warranty_end_date){
    my_new_operation_date = new Date(new Date(my_new_operation_date).setMonth(my_new_operation_date.getMonth()+mymonth));//my_new_operation_date.addMonths(mymonth) ;
  operation_date =  $.datepicker.formatDate('dd/m/yy', my_new_operation_date);//$.format.date(my_new_operation_date, 'dd/M/yy');//my_new_operation_date.toLocaleFormat('%d/%m/%Y');
    dateString =  $.datepicker.formatDate('m/dd/yy', my_new_operation_date);//$.format.date(my_new_operation_date, 'dd/M/yy');//my_new_operation_date.toLocaleFormat('%m/%d/%Y');
    dateString = new Date(dateString);
    dateString = parseInt(dateString.getTime() / 1000);
    calibrations_html = calibrations_html + '<input type="radio" id="'+i+'" name="radiobuttons" value="'+ operation_date +'">'+operation_date+'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    i++;
  }
  $('#calibrations_item').html(calibrations_html);
  document.getElementById("1").checked = true;
  var first_radio = $('#1').val();
  $('#edit-field-calibration-plan-dates-0-value-datepicker-popup-0').val(first_radio);
  $('input[name="radiobuttons"]').change(function(){
    var radiovalue = $('input[name="radiobuttons"]:checked').val();
    $('#edit-field-calibration-plan-dates-0-value-datepicker-popup-0').val(radiovalue);
  }); 
}
