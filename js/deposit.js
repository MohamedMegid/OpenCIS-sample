count = 1;
cnt = 1;
deposit_form = '<div class="errorMsg" style="border:solid; background-color:red;display:none;"></div><fieldset><legend>Item NO.'+count+'</legend><div class="form-item" id="edit-field-test-0-value-wrapper"><label for="edit-field-test-0-value">Item Code: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><select name="deposit_op_type[]" class="form-select css modified_by_amr" id="edit-field-op-type-value"><option value="">Select Op_Type</option><option value="Maintenance">Maintennce</option><option value="Consumed">Consumed</option><option value="Preventive">Preventive</option></select></div><div class="form-item" id="edit-field-number-items-0-value-wrapper"><label for="edit-field-number-items-0-value">Number Of Items: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><input type="text" maxlength="10" rel="num_items_'+count+'" name="deposit_number_of_items[]" id="edit-field-number-items-0-value" size="12" value="" class="form-text css modified_by_amr number"></div><div class="form-item" id="edit-field-unit-price-0-value-wrapper"><label for="edit-field-unit-price-0-value">Unit Price: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><input type="text" class="css" maxlength="10" rel="'+count+'" name="deposit_unit_price[]" id="edit-field-unit-price-0-value" size="12" value="" class="form-text modified_by_amr number"></div><div class="form-item" id="edit-field-tota-price-0-value-wrapper"><label for="edit-field-tota-price-0-value">Total Price: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><input type="text" maxlength="10" rel="total_'+count+'" name="deposit_total_price[]" id="edit-field-tota-price-0-value" size="12" value="" class="form-text modified_by_amr number" readonly></div><div class="form-item" id="edit-field-op-type-value-wrapper"><label for="edit-field-op-type-value">op_type: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><select name="deposit_op_type[]" class="form-select css modified_by_amr" id="edit-field-op-type-value"><option value="">Select Op_Type</option><option value="Maintenance">Maintennce</option><option value="Consumed">Consumed</option><option value="Preventive">Preventive</option></select></div></fieldset>';
save_spare_parts_button = '<div id="btns"><input type="button" value="Save Spare Parts To Store" class="form-submit save-items" style="margin-left: 163px;">';
extra_item_button = '<input type="button" value="+" class="form-submit add-new-item" style="margin-left: 130px;" onclick="addNewItem();"></div>';
$(".link-field-subrow").after(deposit_form + save_spare_parts_button + extra_item_button);

if($(".with-tabs:contains('Work Order')").html()) {
  import_code = '';
}
else {
 import_code = $('input#edit-title').val();
}
$("input#edit-field-link-0-url").next().remove();
$(".link-field-subrow").remove();
	
function addNewItem() {
  count++;
  field_set = '<fieldset><legend>Item NO.'+count+'</legend>';
  item_description = "<div class='form-item' id='edit-field-test-0-value-wrapper'><label for='edit-field-test-0-value'>Item Code: <span class='form-modified_by_amr' title='This field is modified_by_amr.'>*</span></label><input type='text' name='deposit_item_description[]' id='edit-field-test-0-value' size='60' value='' class='form-text css modified_by_amr text'></div>";
  number_of_items = '<div class="form-item" id="edit-field-number-items-0-value-wrapper"><label for="edit-field-number-items-0-value">Number Of Items: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><input type="text" maxlength="10" rel="num_items_'+count+'" name="deposit_number_of_items[]" id="edit-field-number-items-0-value" size="12" value="" class="form-text css modified_by_amr number"></div>';
  unit_price = '<div class="form-item" id="edit-field-unit-price-0-value-wrapper"><label for="edit-field-unit-price-0-value">Unit Price: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><input type="text" class="css" maxlength="10" rel="'+count+'" name="deposit_unit_price[]" id="edit-field-unit-price-0-value" size="12" value="" class="form-text modified_by_amr number"></div>';
  total_price = '<div class="form-item" id="edit-field-tota-price-0-value-wrapper"><label for="edit-field-tota-price-0-value">Total Price: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><input type="text" maxlength="10" rel="total_'+count+'" name="deposit_total_price[]" id="edit-field-tota-price-0-value" size="12" value="" class="form-text modified_by_amr number" readonly></div>';
  op_type = '<div class="form-item" id="edit-field-op-type-value-wrapper"><label for="edit-field-op-type-value">op_type: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><select name="deposit_op_type[]" class="form-select css modified_by_amr" id="edit-field-op-type-value"><option value="">Select Op_Type</option><option value="Maintenance">Maintennce</option><option value="Consumed">Consumed</option><option value="Preventive">Preventive</option></select></div>';
  field_set_end = '</fieldset>';
  new_form = field_set + item_description + number_of_items + unit_price + total_price + op_type + field_set_end;  
  $('#btns').before(new_form);
}

$("input[name='deposit_unit_price[]']").live('keyup', function() {
  totalPrice = parseInt($(this).val()) * parseInt($("input[rel='num_items_"+$(this).attr('rel')+"']").val());
  $("input[rel='total_"+$(this).attr('rel')+"']").attr("value", totalPrice);
});

$('.save-items').click(function() {
  var item_descriptions = new Array();
  var number_items = new Array();
  var unit_prices = new Array();
  var op_types = new Array();
  var errorMsg = "<ul>";
  var success = true;
  $(".css").css("border", "solid black 1px");
  $(".errorMsg").html("");
  $(".errorMsg").hide();
  $("input[name='deposit_item_description[]']").each(function(i) {
    if($(this).val() == '') {
      j = i+1;
      errorMsg += "<li>Item Code for Item NO."+j+"</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else {
      item_descriptions[i] = $(this).val();
    }
  });
  $("input[name='deposit_number_of_items[]']").each(function(i) {
    if($(this).val() == '') {
      j = i+1;
      errorMsg += "<li>Number Of Items for Item NO."+j+"</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else if(isNaN($(this).val())) {
      j = i+1;
      errorMsg += "<li>Number Of Items for Item NO."+j+" must be a valid Number</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else {
      number_items[i] = $(this).val();
    }
  });
  $("input[name='deposit_unit_price[]']").each(function(i) {
    if($(this).val() == '') {
      j = i+1;
      errorMsg += "<li>Unit Price for Item NO."+j+" is modified_by_amr</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else if(isNaN($(this).val())) {
      j = i+1;
      errorMsg += "<li>Unit Price for Item NO."+j+" must be a valid Number</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else {
      unit_prices[i] = $(this).val();
    }
  });
  $("select[name='deposit_op_type[]']").each(function(i) {
    if($(this).val() == '') {
      j = i+1;
      errorMsg += "<li>Please select Op Types for Item NO."+j+"</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else {
      op_types[i] = $(this).val();
    }
  });
  if(!success) {
    errorMsg += "</ul>";
    $(".errorMsg").html(errorMsg);
    $(".errorMsg").show();
    return false;
  }
  res = window.confirm("Are you sure you want to save these items to store? ");
  if(res == true) {
    $("body").append("<div id='overlay'></div>");
    $("#overlay").height($(document).height()).css({
      'opacity' : 0.4,
      'position': 'absolute',
      'top': 0,
      'left': 0,
      'background-color': 'black',
      'width': '100%',
      'z-index': 5000
    });
    if(!import_code) {
      equ = ($("#edit-field-service-report-equip-nid-nid").val()).split(" ");
      hospital_id = $("#edit-field-service-report-hospital-nid-nid").val();
    }
    else {
      equ = ($("#edit-field-importcode-equipment-nid-nid").val()).split(" ");
    hospital_id = $("#edit-field-importcode-hospital-nid-nid").val();
    }
    equipment_nid = equ[1].split(":");
    equipment_nid = equipment_nid[1].substring(0, equipment_nid[1].length - 1);
    //equipment_name_id = equ[0];
    spare_part_items = JSON.stringify({import_code: import_code, equipment_nid: equipment_nid, hospital_id: hospital_id, item_descriptions: item_descriptions, number_items: number_items, unit_prices: unit_prices, op_types: op_types});
    $.post(Drupal.settings.basePath + "spare_parts_deposit", {postData:spare_part_items}, function(response) {
      result = JSON.parse(response);
      $("#overlay").remove();
      if(result[0]) {
        alert("Spare Parts Saved successfully to Store.");
      }
      else {
       alert("Error, Items from No."+result[1]+" didn't saved to store.");
      }
    });
  }
});

/*withdraw*/
title = "<h3>Withdraw Spare Parts From Store</h3>";
withdraw_fieldset = "<fieldset><legend>Item No."+cnt+"</legend>";
var options = $("#edit-field-item-description-list-value").html();
withdraw_num_items = '<div class="form-item" id="edit-field-number-item-0-value-wrapper"><label for="edit-field-number-item-0-value">Number Of Item: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><input type="text" maxlength="10" name="field_number_item[0][value]" id="edit-field-number-item-0-value" size="12" value="" class="form-text css modified_by_amr number"></div>';
withdraw_op = '<div class="form-item" id="edit-field-op-type-withdraw-value-wrapper"><label for="edit-field-op-type-withdraw-value">op_type: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><select name="field_op_type_withdraw[value]" class="form-select css modified_by_amr" id="edit-field-op-type-withdraw-value"><option value="" selected="selected">select op_type</option><option value="Maintenance">Maintennce</option><option value="Consumed">Consumed</option><option value="Preventive">Preventive</option></select></div>';
withdraw_button = '<div id="btns_2"><input type="button" value="Withdraw Spare Parts From Store" class="form-submit withdraw-items" style="margin-left: 128px;">';
extra_item_button_2 = '<input type="button" value="+" class="form-submitadd-new-item" style="margin-left: 130px;" onclick="addWithdraw();"></div>';
err_div = '<div class="errMsg" style="border:solid; background-color:red;display:none;">hghghg</div>'
$('#edit-field-item-description-list-value-wrapper').before(title + err_div + withdraw_fieldset);
$("#edit-field-item-description-list-value-wrapper").after(withdraw_num_items + withdraw_op + '</fieldset>' + withdraw_button + extra_item_button_2);

function addWithdraw() {
  cnt++;
  withdraw_fieldset = "<fieldset><legend>Item No."+cnt+"</legend>";;
  items_desc = '<div class="form-item" id="edit-field-item-description-list-value-wrapper"><label for="edit-field-item-description-list-value">Item Code: <span class="form-modified_by_amr" title="This field is modified_by_amr.">*</span></label><select name="field_item_description_list[value]" class="form-select modified_by_amr" id="edit-field-item-description-list-value">'+options+'</select></div>';
  $("#btns_2").before(withdraw_fieldset + items_desc + withdraw_num_items + withdraw_op + "</fieldset>");
}

$(".withdraw-items").click(function() {
  var item_descriptions = new Array();
  var number_items = new Array();
  var op_types = new Array();
  var errorMsg = "<ul>";
  var success = true;
  $(".css").css("border", "solid black 1px");
  $(".errMsg").html("");
  $(".errMsg").hide();
  $("select[name='field_item_description_list[value]']").each(function(i) {
    item_descriptions[i] = $(this).val();
  });

  $("input[name='field_number_item[0][value]']").each(function(i) {
    if($(this).val() == '') {
      j = i+1;
      errorMsg += "<li>Number Of Items for Item NO."+j+" is modified_by_amr</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else if(isNaN($(this).val())) {
      j = i+1;
      errorMsg += "<li>Number Of Items for Item NO."+j+" must be a valid Number</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else {
      number_items[i] = $(this).val();
    }
  });
  $("select[name='field_op_type_withdraw[value]']").each(function(i) {
    if($(this).val() == '') {
      j = i+1;
      errorMsg += "<li>Please select Op Types for Item NO."+j+"</li>";
      $(this).css("border", "solid red");
      success = false;
    }
    else {
      op_types[i] = $(this).val();
    }
  });
  if(!success) {
    errorMsg += "</ul>";
    $(".errMsg").html(errorMsg);
    $(".errMsg").show();
    return false;
  }
  res = window.confirm("Are you sure you want to withdraw these items from store? ");
  if(res == true) {
    $("body").append("<div id='overlay'></div>");
    $("#overlay").height($(document).height()).css({
      'opacity' : 0.4,
      'position': 'absolute',
      'top': 0,
      'left': 0,
      'background-color': 'black',
      'width': '100%',
      'z-index': 5000
    });
    spare_part_items = JSON.stringify({item_descriptions: item_descriptions, number_items: number_items, op_types: op_types});
    $.post(Drupal.settings.basePath + "spare_parts_withdraw", {postData:spare_part_items}, function(response) {
      result = JSON.parse(response);
      $("#overlay").remove();
      if(result[0]) {
        alert("Saved Successfully.");
      }
      else {
       alert("Error while processing from Item No."+result[1]);
      }
    });
  }
});
/*end*/
