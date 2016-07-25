Drupal.behaviors.cis_ppm_cal_dates = function() {
  $(document).ready(function() {
    // HIDE/Show Functionality
    var filterslabel = "Hide/Show filters";
    $('#edit-field-custom-0-value-wrapper').next().hide();
    $( ".view-filters" ).before( "<div id ='ofilters'>"+ filterslabel+"</div>" );
    $('.view-filters').show();
    var filters_shown = true;
    $("#ofilters").click(function() { 
      if (filters_shown == false) {
        $('.view-filters').show();
        filters_shown = true;
      }
      else {   
        $('.view-filters').fadeOut(100);
        filters_shown = false;
      }
    });

    // put tasks count in the calendar items
    _calendar_item_tasks_count();
    manage_fields_for_quotation_for_contract();
    total_price_form_elements();
    qty_requested_span_in_invoice();
    set_assigned_emp_on_task_calendar();
    set_task_review_in_pentry();
    set_task_review_in_work_order();
    hide_resolved_status_if_need_quotation_in_call();
    modify_call_last_situation_in_po();
    modify_call_last_situation_in_quotation();
    modify_call_last_situation_in_work_order();
    hide_mcontract_start_date();
    check_all_in_update_equipment_information_page();
    set_count_of_notification_based_on_not_readed_tasks();
    check_all_for_all_messages_notification_page();
    //set_department_select_list_options_in_equipment();
    set_all_tasks_for_specific_user_as_readed_tasks();

    // close button to hide total price form
    $('.close_price_form').click(function(event){
      $(this).parent().parent().hide();
    });

    // show total price form @ quotation
    $('.total_price').click(function(event) {
      $('.popbox').hide();
      $(this).parent().parent().parent().next().show();
      var unit_price_popup_value  = $(this).parent().parent().parent().prev().children().children().children().children().attr("id");
      $('.unit_price').val(unit_price_popup_value);
    });


    // save button to calculate values then set the value of each unit price (total price form)
    $('.save_price_form').click(function(event){
      var unit_price     = $(this).parent('').parent('').prev('').children('').attr("id");
      unit_price         = unit_price.replace('-wrapper', '');
      var unit_field     = unit_price;
      unit_price         = $('#' + unit_price).val();
      var unit_price_equation = $(this).parent().prev().prev().prev().prev().prev().prev().prev().children().val();
      var agent_discount = $(this).parent().prev().prev().prev().prev().prev().prev().children().val();
      agent_discount     = (agent_discount / 100) * unit_price_equation;
      var factor_f5      = $(this).parent().prev().prev().prev().prev().prev().children().val();
      var factor_exchange = $(this).parent().prev().prev().prev().prev().children().val();
      var profit         = $(this).parent().prev().prev().prev().children().val();
      var taxes          = $(this).parent().prev().prev().children().val();
      //$.get(Drupal.settings.basePath +'get/factor/exchange/value',null, function(response) {
      var total_price = (unit_price_equation - agent_discount) * factor_exchange * factor_f5 * profit;
        
      //  factor_f5          = (factor_f5 / 100) * unit_price_equation;
      //  profit             = (profit / 100) * unit_price_equation;
      //  var total_price = unit_price_equation + factor_f5 + profit;
      if (taxes == 1){
        // with taxes add 10% of unit price to total price
        var tax_10 = (total_price * 10 ) / 100;
        total_price = total_price + tax_10;
      }
      $('#' + unit_field).val(total_price.toFixed(2));
      //});
      $(this).parent().parent().hide();
    });

    // show maintenance info tab based on first action tab
    $('#edit-field-job-first-action-status-value').change(function(event) {
      var first_action_status = $('#edit-field-job-first-action-status-value').val();
      $.get(Drupal.settings.basePath +'maintenance/info/tab',null, function(response) {
        if (response == 1){
          if (first_action_status == 4 || first_action_status == 3){
            $('.horizontal-tab-button-1').attr("style", "display : block !important");
          }
          else{
            $('.horizontal-tab-button-1').attr("style", "display : none !important");
          }
        }
      });
    });


    // disable qty received in purchase order
    $( "input[id*='field-importcode-qty-received-value']" ).attr('disabled', 'disabled');

    // margin top to items count in quotation
    $( "input[id*='field-quotaion-items-count-value']" ).css('margin-top', '-3px');

    // mobile & mail for contact person @ client
    $( "input[id*='field-client-contact-person-mobi-value']" ).css('width', '65%');
    $( "input[id*='field-client-contact-person-mail-value']" ).css('width', '65%');


    // disable qty requested in invoice
    //$("input[id*='field-invoice-qty-requested-value']").attr('disabled', 'disabled');

    // disable description in invoice
    //$("input[id*='field-invoice-part-name-value']").attr('disabled', 'disabled');

    // attach  user location to node, in cck user location field
    $('#edit-field-user-location-0-value-wrapper').hide();
       //FILL GROUP LEADER AUTOMATIC @ CALL
    $('#edit-field-job-client-0-nid-nid').blur(function(event) {
      var client = $('#edit-field-job-client-0-nid-nid').val();
      var split = client.split(":");
      var client_nid = split[1];
      client_nid = client_nid.substr(0,client_nid.length-1);
      if (client_nid){
        $.get(Drupal.settings.basePath +'fill/group/leader/auto/'+client_nid+'',null, function(response) {
          $('#edit-field-job-group-leader-uid-uid').val(response);
        });
      }
      else if (!client_nid){
        $('#edit-field-job-group-leader-uid-uid').val('');
      }
    });
    add_edit_form_to_calendar_item();
    color_tasks();
    $('.calendar_item_close_btn').click(function () {
      $(this).parent().parent().hide();
    });
    $('.show_calendar_item_edit_form').click(function () {
      $(this).next().show();
        // get list of employees
        var items="<option value=0>None</option>";;
        $.getJSON(Drupal.settings.basePath +"get/employees",function(data){
          $.each(data,function(index,item) {
            items+="<option value='"+item.uid+"'>"+item.name+"</option>";
          });
          $(".list_employees").html(items);
        });
    });
    // save button for calendar item
    $('.calendar_item_save_btn').click(function () {
      var calendar_item_class = $(this).parent().parent().prev().prev().children().attr('class');
      var class_calendar_arr = calendar_item_class.split('.');
      var nid = class_calendar_arr[1];
      var expected_date = $(this).prev().parent().prev().children().val();
      if (!expected_date || expected_date == ''){
        expected_date = '_';
      }
      else{
        expected_date = Date.parse(expected_date).getTime()/1000;
      }
      var employee = $(this).prev().val();
      if (employee == 0){
        employee = '_';
      }
      $.get(Drupal.settings.basePath +'recieve/data/' + nid + '/' + expected_date + '/' + employee, null, function(response){
        if (response == 1){
          alert('Calendar has been updated');
          window.location.reload();
        }
        else {
          alert(response);
        }
      });
    });

    var path = location.pathname.split("/");
    if ( path[2] == "overall" && path[3] == "calendar" ) {
      $( ".datepicker" ).datepicker();
    }
    getLocation();
  });
}

/**
 * set value of task review in pentry
 * related to function set_task_review_value_in_pentry
 */
function set_task_review_in_pentry(){
  $('#reviewed').click(function(){
    var path = location.pathname.split("/");
    var pentry_nid = path[3];
    $.get(Drupal.settings.basePath +'set_task_review_in_pentry/'+pentry_nid+"/"+1, null, function(response){
      if (response == 1){
        window.location.reload();
        response++;
      }
    });
  });
  $('#not_reviewed').click(function(){
    var path = location.pathname.split("/");
    var pentry_nid = path[3];
    $.get(Drupal.settings.basePath +'set_task_review_in_pentry/'+pentry_nid+"/"+2, null, function(response){
      if (response == 1){
        window.location.reload();
        response++;
      }
    });
  });
}

/**
 * set value of task review in work order
 * related to function set_task_review_value_in_work_order
 */
function set_task_review_in_work_order(){
  $('#wrk_order_reviewed').click(function(){
    var path = location.pathname.split("/");
    var work_order_nid = path[3];
    $.get(Drupal.settings.basePath +'set_task_review_in_work_order/'+work_order_nid+"/"+1, null, function(response){
      if (response == 1){
        window.location.reload();
        return;
      }
    });
  });
  $('#wrk_order_not_reviewed').click(function(){
    var path = location.pathname.split("/");
    var work_order_nid = path[3];
    $.get(Drupal.settings.basePath +'set_task_review_in_work_order/'+work_order_nid+"/"+2, null, function(response){
      if (response == 1){
        window.location.reload();
        return;
      }
    });
  });
}

function set_assigned_emp_on_task_calendar() {
  // pass value of employee to assigned pentry & centry @ overall calendar
  $('#edit-field-job-assigned-user-uid').change(function(){
    var employee = $('#edit-field-job-assigned-user-uid').val();
    $('#edit-field-pentry-hospital-person-uid').val(employee);
    $('#edit-field-hospital-person-uid').val(employee);
  });
}

/**
 * function to color the tasks in calendar
 */

function color_tasks(){
  var path = location.pathname.split("/");
  if (path[2] != 'overall'){
    return;
  } 
  var calendar_items_list = $('.view-item-overall_calendar');
  var calendar_items_count = calendar_items_list.length;
  var i =1;
  var item_unique_class = '';
  var task_type = '';
  for ( i =0; i< calendar_items_count; i++) {
    item_unique_class = $(calendar_items_list[i]).children().attr('class');
    task_type = item_unique_class.split('.')[2];
    console.log(task_type);
    // color job with #BE1C56
    if (task_type == 'field_job_expected_date_of_visit') {
      //console.log($(calendar_items_list[i]).children().children().next().attr('class'));
      $(calendar_items_list[i]).children().children().attr("style", "background-color : whitesmoke  !important; color : #3E4FA8");;
      $(calendar_items_list[i]).children().children().next().children().attr("style", "color : #3E4FA8 !important");
    }
    // color ppm with #35AA47
    if (task_type == 'field_pentry_date') {
      //console.log($(calendar_items_list[i]).children().children().next().attr('class'));
      $(calendar_items_list[i]).children().children().attr("style", "background-color : whitesmoke !important; color : #35AA47");;
      $(calendar_items_list[i]).children().children().next().children().attr("style", "color : #35AA47 !important");
    }
    // color calibration with pink
    if (task_type == 'field_centry_date') {
      //console.log($(calendar_items_list[i]).children().children().next().attr('class'));
      $(calendar_items_list[i]).children().children().attr("style", "background-color : whitesmoke !important; color : crimson");;
      $(calendar_items_list[i]).children().children().next().children().attr("style", "color : crimson !important");
    }
  }

}

/**
 * function to add total price form to
 * quotation part item
 */
function total_price_form_elements(){
  var wo_nid;
  var path = location.pathname.split("/");
  if (path[3] == 'add'){
    wo_nid = window.location.search.substring(1);
    var url = wo_nid;
    if (wo_nid != undefined  && wo_nid[1] != undefined) {
      url = url.split("=");
      if (url[0] == 'quotation_equipment'){
        wo_nid = wo_nid.split("&");
        wo_nid = wo_nid[1].split("=");
      }
      wo_nid = wo_nid[1];
    }
  }
  else if (path[4] == 'edit'){
    wo_nid = $('#edit-field-quotation-service-report-nid-nid').val();
    if (wo_nid != undefined  && wo_nid[1] != undefined) {
      wo_nid = wo_nid.split(":");
      wo_nid = wo_nid[1];
      wo_nid = wo_nid.substr(0,wo_nid.length-1);
    }
  }
  if (wo_nid){
    $.get(Drupal.settings.basePath +'unit/price/wo/'+wo_nid+'', null, function(response) {
      var delta = response.split("/");
      delta = delta[0];
      delta = delta[delta.length-2];
      var unit_price = response.split("/");
      unit_price = unit_price[1];
      unit_price = unit_price.split("-");
      var currency = response.split("/");
      currency = currency[2];
      currency = currency.split("-");
      var x = 0;
      for (x; x <= delta; x++){
        if (currency[x] == 0 && currency[x]){
          currency[x] = 'EGP';
        }
        if (currency[x] == 1 && currency[x]){
          currency[x] = '$';
        }
        if (!currency[x]){
          currency[x] = '';
        }
        if (currency[x] == 2 && currency[x]){
          currency[x] = 'Euro';
        }
        if (unit_price[x]){
          var element = '<div class="description"><span class="unit_price_wo"><em id='+unit_price[x]+'>WK Unit price:'+unit_price[x] + currency[x]+'</em></span></div>';
          $('#edit-group-spare-parts-'+x+'-field-quotaion-items-count-value').after(element);
        }
      }
    });
  }
    var total_price_form =
                            '<div id="pop1" class="popbox">'+
                                  '<div class="unit_price_div  clc_txt">'+
                                     "Unit price<input type='textfield' class = 'unit_price'></input>"+
                                  '</div>'+
                                  '<div class="agent_discount_div clc_txt">'+
                                     "Agent disount<input type='textfield' class = 'agent_discount'></input>"+
                                  '</div>'+
                                  '<div class="f5_div clc_txt">'+
                                     "Factor F5<input type='textfield' class = 'f5'></input>"+
                                  '</div>'+
                                  '<div class="fx_div clc_txt">'+
                                     "Factor Ex<input type='textfield' class = 'fx'></input>"+
                                  '</div>'+
                                  '<div class="profit_div clc_txt">'+
                                     "Profit<input type='textfield' class = 'profit'></input>"+
                                  '</div>'+
                                  '<div class="taxes_div clc_txt">'+
                                     "Taxes<select name='taxes_type' class = 'taxes'>"+
                                                  "<option value=1>With taxes</option>"+
                                                  "<option value=2>Without taxes</option>"+
                                     "</select>"+
                                  '</div>'+
                                  '<div class="equation">'+
                                     "Equation : Price = <br>"+
                                     "(Unit price - agent discount%)<br>"+
                                     "* Factor exchange<br>"+
                                     "* Factor f5% * Profit%<br>"+
                                     "+ 10% of unit price<br>"+
                                     "  if(With taxes)"+
                                  '</div>'+
                                  '<div class="buttons">'+
                                     "<input type='button' class ='save_price_form' value='Save'></input>"+
                                     "<input type='button' class ='close_price_form' value='Close'></input>"+
                                  '</div>'+
                            '</div>';
    $('.content-multigroup-cell-field-quotation-unit-price').after(total_price_form);
}


function _calendar_item_tasks_count() {
  var path = location.pathname.split("/");
  if (path[2] == 'ar') {
    path[2] = path[3];
    path[3] = path[4];
  }
  if (path[2] == 'overall'  && path[3] == 'calendar') {
    $('.has-events').each(function () {
      var item_lenght = $(this).find('.view-item-overall_calendar').length;
      $(this).find('.inner').find('.month').after("<span class='tasks_count'>count = <b>" + item_lenght + "</b></span>");  
    });
  }
  else if (path[2] == 'ppmentry'  && path[3] == 'calendar'){
    $('.has-events').each(function () {
      var item_lenght = $(this).find('.view-item-ppmentry_schedule').length;
      $(this).find('.inner').find('.month').after("<span class='tasks_count'>count = <b>" + item_lenght + "</b></span>");  
    });
  }
}

/**
 * function to add edit form to 
 * calendar item
 */
function add_edit_form_to_calendar_item() {

  var form_html =   '<span class="show_calendar_item_edit_form"> edit</span>'+
                    '<div class = "calendar_item_edit">'+
                      '<p>Expected Date: <input type="text" class="datepicker"></p>' +
                      '<p>Employee: <select class="list_employees">'+
                      '</select>'+
                      '<button class="calendar_item_save_btn" type="button">Save!</button>'+
                      '<button class="calendar_item_close_btn" type="button">Close!</button>'+
                    '</div>'
                  ;
  
  $('.view-item-overall_calendar').after( form_html);

}

/**
 * get ordered qty @ invoice
 * (qty requested of PO - qty received of PO)
 */
function qty_requested_span_in_invoice(){
  var po_nid;
  var path = location.pathname.split("/");
  if (path[3] == 'add'){
    po_nid = window.location.search.substring(1);
    var url = po_nid;
    if (po_nid != undefined  && po_nid[1] != undefined) {
      url = url.split("=");
      if (url[0] == 'invoice_call'){
        po_nid = po_nid.split("&");
        po_nid = po_nid[2].split("=");
        po_nid = po_nid[1];
      }
    }
  }
  else if (path[4] == 'edit'){
    po_nid = $('#edit-field-invoice-po-number-nid-nid').val();
    if (po_nid != undefined  && po_nid[1] != undefined) {
      po_nid = po_nid.split(":");
      po_nid = po_nid[1];
      po_nid = po_nid.substr(0,po_nid.length-1);
    }
  }
  if (po_nid){
    $.get(Drupal.settings.basePath +'get/qty/requested/received/po/'+po_nid+'', null, function(response) {
      var result = Drupal.parseJson(response);
      var i = 0;
      for (var remained_qty in result) {
        // important check that this is objects own property
        // not from prototype prop inherited
        if(result.hasOwnProperty(remained_qty)){
          //alert(prop + " = " + result[prop]);
          var element = '<div class="description"><span class="ordered_qty_invoice">Remained Qty:'+result[remained_qty]+'</span></div>';
          $('#edit-group-spare-parts-'+i+'-field-invoice-qty-recieved-value').after(element);
        }
        i++;
      }
      if (result && remained_qty){
        console.log(result);
      }
    });
  }
}

/**
 * get location of user using
 * navigator object
 */
function getLocation() {
  if (navigator.geolocation) {
    navigator.geolocation.getCurrentPosition(showPosition);
  }
  else { 
    x.innerHTML = "Geolocation is not supported by this browser.";
    $('#edit-field-user-location-0-value-wrapper').after("<em><span id='user_loc'><b>User Location:</b> Geolocation is not supported by this browser </span></em>");
  }
}

/**
 * helper function to sown the position in the 
 * selected fields
 */
function showPosition(position) {
  $('#edit-field-user-location-0-value').val(position.coords.latitude + "," + position.coords.longitude);
  var request = new XMLHttpRequest();
  var method = 'GET';
  var url = 'http://maps.googleapis.com/maps/api/geocode/json?latlng='+position.coords.latitude+','+position.coords.longitude+'&sensor=true';
  var async = false;
  request.open(method, url, async);
  request.onreadystatechange = function(){
    if(request.readyState == 4 && request.status == 200){
      var data = JSON.parse(request.responseText);
      var address = data.results[0];
      $('#edit-field-user-location-0-value').val(address.formatted_address);
      $('#user_loc').remove();
      $('#edit-field-user-location-0-value-wrapper').after("<em><span id='user_loc'><b>User Location:</b> "+address.formatted_address + "</span></em>");
    }
  };
  request.send();
}

/**
 * manage fields for maintenance contract
 * if the contract begin as a contract or quotation
 */
function manage_fields_for_quotation_for_contract(){
  // hide type of contract otherwise it is (contract or quotation) by default if it is not configured
  $('#edit-field-mcontracts-type-config-value-wrapper').hide();

  // set fields based on contract or quotation for contract specially for (edit case)
  var contract_begin_as = $('#edit-field-mcontracts-type-config-value').val();
  if (contract_begin_as == 2){
    $('#edit-field-mcontracts-type-config-value-wrapper').show();
    $('#group-quotation-for-contract-mod-items').show();
    $('#edit-field-mcontracts-end-date-0-value-wrapper').attr("style", "display : none !important");
    $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0-wrapper').attr("style", "display : none !important");
    $('#popups-reference-0').attr("style", "display : none !important");
    $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0').val(null);
    $('#edit-field-mcontracts-end-date-0-value2-datepicker-popup-0').val(null);
  }
  else{
    $('#group-quotation-for-contract-mod-items').hide();
    $('#edit-field-mcontracts-end-date-0-value-wrapper').attr("style", "display : block !important");
    $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0-wrapper').attr("style", "display : block !important");
    $('#popups-reference-0').attr("style", "display : block !important");
  }

  // hide and show start date of contract if contract begin as a quotation (for checkbox)
  $('#edit-field-mcontracts-begin-as-quotat-value').change(function(event){
    if(this.checked) {
      $('#edit-field-mcontracts-type-config-value-wrapper').show();
      $('#edit-field-mcontracts-type-config-value').val(2);
    }
    else{
      $('#edit-field-mcontracts-type-config-value').val('');
    }
    var type_config = $('#edit-field-mcontracts-type-config-value').val();
    if (type_config == 2){
      $('#group-quotation-for-contract-mod-items').show();
      $('#edit-field-mcontracts-end-date-0-value-wrapper').attr("style", "display : none !important");
      $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0-wrapper').attr("style", "display : none !important");
      $('#popups-reference-0').attr("style", "display : none !important");
      $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0').val(null);
      $('#edit-field-mcontracts-end-date-0-value2-datepicker-popup-0').val(null);
    }
    else{
     $('#group-quotation-for-contract-mod-items').hide();
      $('#edit-field-mcontracts-end-date-0-value-wrapper').attr("style", "display : block !important");
      $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0-wrapper').attr("style", "display : block !important");
      $('#popups-reference-0').attr("style", "display : block !important");
    }
  });

  // hide and show start date of contract if contract begin as a quotation (type select list)
  $('#edit-field-mcontracts-type-config-value').change(function(event){
    var type_config = $('#edit-field-mcontracts-type-config-value').val();
    if (type_config == 2){
      $('#group-quotation-for-contract-mod-items').show();
      $('#edit-field-mcontracts-end-date-0-value-wrapper').attr("style", "display : none !important");
      $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0-wrapper').attr("style", "display : none !important");
      $('#popups-reference-0').attr("style", "display : none !important");
      $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0').val(null);
      $('#edit-field-mcontracts-end-date-0-value2-datepicker-popup-0').val(null);
    }
    else{
      $('#group-quotation-for-contract-mod-items').hide();
      $('#edit-field-mcontracts-end-date-0-value-wrapper').attr("style", "display : block !important");
      $('#edit-field-mcontracts-end-date-0-value-datepicker-popup-0-wrapper').attr("style", "display : block !important");
      $('#popups-reference-0').attr("style", "display : block !important");
    }
  });
}

/**
 * hide resolved option from select list status in maintenance info tab
 * based on option of need a quotation from first action select 
 * in first action tab @ CALL
 */
function hide_resolved_status_if_need_quotation_in_call(){
  $('#edit-field-job-first-action-status-value').change(function(event){
    var first_action_select = $('#edit-field-job-first-action-status-value').val();
    if (first_action_select == 4){ // Need A quotation option
      $('#edit-field-job-status-value option[value=6]').attr("style", "display : none !important");
    }
    else{
      $('#edit-field-job-status-value option[value=6]').attr("style", "display : block !important");
    }
  });
}

/**
 * modify call last situation select list 
 * in (PO)
 */
function modify_call_last_situation_in_po(){
  //remove options from call's last situations in P.O
  $('#edit-field-importcode-m-situtation-value option[value=1]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=2]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=3]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=4]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=5]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=6]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=7]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=8]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=9]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=12]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=13]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=14]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=15]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=16]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=17]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=18]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=22]').attr("style", "display : none !important");
  $('#edit-field-importcode-m-situtation-value option[value=23]').attr("style", "display : none !important");
}

/**
 * modify call last situation select list
 * in (Quotation)
 */
function modify_call_last_situation_in_quotation(){
  //remove options from call's last situations in quotations
  $('#edit-field-quotation-m-situation-value  option[value=1]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=2]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=3]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=4]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=5]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=6]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=7]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=10]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=11]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=12]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=13]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=14]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=15]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=17]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=18]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=20]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=21]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=22]').attr("style", "display : none !important");
  $('#edit-field-quotation-m-situation-value  option[value=23]').attr("style", "display : none !important");
}

/**
 * modify call last situation select list
 * based on action of the status select list
 * in (Work Order)
 */
function modify_call_last_situation_in_work_order(){
  var status_option=$('#edit-field-job-status-value').val();
  if (status_option == 4 || status_option == 5){
    $('#edit-field-wrk-order-m-situation-value option[value=4]').attr("style", "display : none !important");
    $('#edit-field-wrk-order-m-situation-value option[value=9]').attr("style", "display : none !important");
    $('#edit-field-wrk-order-m-situation-value option[value=10]').attr("style", "display : none !important");
    $('#edit-field-wrk-order-m-situation-value option[value=13]').attr("style", "display : none !important");
    $('#edit-field-wrk-order-m-situation-value option[value=16]').attr("style", "display : none !important");
    $('#edit-field-wrk-order-m-situation-value option[value=22]').attr("style", "display : none !important");
    $('#edit-field-wrk-order-m-situation-value option[value=23]').attr("style", "display : none !important");
    $('#edit-field-wrk-order-m-situation-value option[value=17]').attr("style", "display : none !important");
  }
  // on change of the status select list
  $('#edit-field-job-status-value').change(function(event){
    var status_option=$('#edit-field-job-status-value').val();
    // if the selected option is 4 => NEW || 5 => ASSIGNED
    if(status_option == 4 || status_option == 5){
      // hide the specific options
      $('#edit-field-wrk-order-m-situation-value option[value=4]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value option[value=9]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value option[value=10]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value option[value=13]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value option[value=16]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value option[value=22]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value option[value=23]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value option[value=17]').attr("style", "display : none !important");
      // in case of choose new then resolved then new again, need to show these options
      $('#edit-field-wrk-order-m-situation-value option[value=1]').attr("style", "display : block !important");
      $('#edit-field-wrk-order-m-situation-value option[value=2]').attr("style", "display : block !important");
      $('#edit-field-wrk-order-m-situation-value option[value=5]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=6]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=7]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=8]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=11]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=16]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=18]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=20]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=21]').attr("style", "display : block  !important");
    }
    // else if the selected option is 6 => RESOLVED
    else if(status_option == 6){
      $('#edit-field-wrk-order-m-situation-value  option[value=1]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=2]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=4]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=5]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=6]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=7]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=8]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=9]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=10]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=11]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=13]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=16]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=17]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=18]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=20]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=21]').attr("style", "display : none !important");
      $('#edit-field-wrk-order-m-situation-value option[value=22]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=23]').attr("style", "display : block  !important");
    }
    // if the selected option resolved then others like repeated, need to show the hidden options
    else {
      $('#edit-field-wrk-order-m-situation-value  option[value=1]').attr("style", "display : block !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=2]').attr("style", "display : block !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=4]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=5]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=6]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=7]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=8]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=9]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=10]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=11]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=13]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=16]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=17]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=18]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=20]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value  option[value=21]').attr("style", "display : block  !important");
      $('#edit-field-wrk-order-m-situation-value option[value=22]').attr("style", "display : block !important");
      $('#edit-field-wrk-order-m-situation-value option[value=23]').attr("style", "display : block !important");
    }
  });
  // hide non-response and need-reassigned from call last situation select list
  $('#edit-field-wrk-order-m-situation-value option[value=3]').attr("style", "display : none !important");
  $('#edit-field-wrk-order-m-situation-value option[value=12]').attr("style", "display : none !important");

  // add none response option to status select list
  var check_if_wo = $('#edit-field-job-status-value').parent().parent().next().attr('id');
  if (check_if_wo == 'conditional-field-wrk-order-m-situation'){
    $('#edit-field-job-status-value').append($("<option value='45'>None response</option>"));
  }
}

/**
 * hide start date of m contract in contract view
 * if empty the strat date as it get 1/1/1970
 */
function hide_mcontract_start_date(){
  var start_date = $('.date-display-end').html();
  if (start_date == '01/01/1971'){
    $('.date-display-end').hide();
    $('.field-label-inline-first').hide();
    $('.date-display-separator').hide();
  }
}

/**
 * set department select list options according 
 * to language (arabic - english) 
 * in equipment
 */
function set_department_select_list_options_in_equipment(){
  var items="<option value=0>None</option>";
  var path = location.pathname.split("/");  
  $.getJSON(Drupal.settings.basePath +"set/department/options/in/equipment/" + path[2],function(data){
    $.each(data,function(index,item) {
      items+="<option value='"+item.nid+"'>"+item.name+"</option>";
    });
    $("#edit-field-equipment-department-nid-nid").html(items);
  });
}

/**
 * check All equipment in update equipment information page
 */
function check_all_in_update_equipment_information_page(){
  $('#equ_checkall').change(function(event) {
    if(this.checked) {
      $('.equ_checkbox1').each(function() {
        this.checked = true;
      });
    }
    else{
      $('.equ_checkbox1').each(function() {
        this.checked = false;
      });
    }
  });
}

/**
 * check All tasks related to all messages page
 */
function check_all_for_all_messages_notification_page(){
  $('#tasks_checkall').change(function(event) {
    if(this.checked) {
      $('.task_checklist').each(function() {
        this.checked = true;
      });
    }
    else{
      $('.task_checklist').each(function() {
        this.checked = false;
      });
    }
  });
}


/**
 * set count of notifications based on
 * not readed tasks
 */
function set_count_of_notification_based_on_not_readed_tasks(){
  $(".stylish").click(function() {
    var task_id = this.id;
    $.get(Drupal.settings.basePath +'notifications/update/count/'+task_id+'', null, function(response) {
      if (response == 1){
        window.location.reload();
      }
    });
  });
}

/**
 * set all tasks for specific user
 * as readed tasks (ajax call)
 */
function set_all_tasks_for_specific_user_as_readed_tasks(){
  $('#tasks_checklist_btn').click(function(event) {
    $.get(Drupal.settings.basePath +'set/tasks/readed',null, function(response) {
      if (response == 1){
        window.location.reload();
      }
    });
  });
}
