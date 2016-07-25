<?php
/**
 * @author Amr Hesham 30/8/2014 
 * @file cis_system_search_forms.inc
 * This file  is includes all forms for search and other form apis FAPI's
 */

/**
 * search equipment maintenance costs
 * render maintenance_costs_form
 */
function search_maintenance_costs(){
  $equipment     = $_GET['equipment'];
  $model         = $_GET['model'];
  $contract      = $_GET['contract'];
  $contract_type = $_GET['contract_type'];
  $date_from     = $_GET['date_from'];
  $date_to       = $_GET['date_to'];
  $where         = " WHERE contract_type < 5 ";
  if (!empty($equipment)){
    $equipment_nid = db_result(db_query('SELECT nid FROM node WHERE title = "%s"', $equipment));
    $where .= " AND cost.equipment_nid = $equipment_nid";
  }
  if (!empty($model)){
    $where .= " AND term_data.name = '{$model}'";
  }
  if(!empty($contract)){
    $contract_nid = db_result(db_query('SELECT nid FROM node WHERE title = "%s"', $contract));
    if (!empty($contract_nid)){
      $where .= " AND cost.contract_nid = $contract_nid";
    }
  }
  if(!empty($contract_type)){
    $where .= " AND cost.contract_type = $contract_type";
  }
  if(!empty($date_from) && !empty($date_to)){
    $where .= " AND (( cost.from_date <= $date_from AND $date_from <= cost.to_date) OR ( cost.from_date < $date_to AND $date_to <= cost.to_date) OR ( cost.from_date >= $date_from AND $date_to >= cost.to_date))";
  }
  $query = 'SELECT term_data.name model_name, cost.equipment_nid equipment_nid, cost.contract_nid contract_nid, cost.contract_type contract_type, cost.from_date from_date, cost.to_date to_date, cost.month_cost month_cost FROM equipmentmaintenance_cost cost INNER JOIN content_field_type_l2 model ON model.nid = cost.equipment_nid INNER JOIN term_data ON term_data.tid = model.field_type_l2_value' . $where;
  $result = pager_query($query, 50);
  $rows = array();
  $header = array("Equipment", "Model", "View Link", "Income type", "Date from", "Date to", "Number of months", "Month Value", "Period Value");
  while ($data = db_fetch_object($result)){
    $equipment_path = url('node/' . $data->equipment_nid, array('query' => array('destination' => 'search/maintenance/costs'))) ;
    $contract_path = url('node/' . $data->contract_nid, array('query' => array('destination' => 'search/maintenance/costs'))) ;
    $row = array();
    $equipment_title = _get_node_title($data->equipment_nid);
    $contract_title = _get_node_title($data->contract_nid);
    $row[] = array('data' => "<a href = '".$equipment_path."'>$equipment_title</a>");
    $row[] = array('data' => $data->model_name);
    if (!empty($data->contract_nid)){
      $row[] = array('data' => "<a href = '".$contract_path."'>$contract_title</a>");
    }
    else{
      $row[] = "N/A";
    }
    if ($data->contract_type == 1){
      $row[] = "Under Warranty";
    }
    else if ($data->contract_type == 2){
      $row[] = "Under Extended Warranty";
    }
    else if ($data->contract_type == 3){
      $row[] = "Under Maintenance Contract";
    }
    else if ($data->contract_type == 4){
      $row[] = "Purchase Order";
    }
    if (empty($data->from_date)){
      $row[] = 'N/A';
    }
    else if ($data->from_date >= $date_from){
      $row[] = array('data' => date('d-m-Y', $data->from_date));
      $from = $data->from_date;
    }
    else{
      $row[] = array('data' => date('d-m-Y', $date_from));
      $from = $date_from;
    }
    if (empty($data->to_date) || $data->to_date < 5000000){
      $row[] = 'N/A';
    }
    else if ($data->to_date >= $date_to && !empty($date_to)){
      $row[] = date('t-m-Y', $date_to);
      $to = $date_to;
    }
    else {
      $row[] = array('data' => date('t-m-Y', $data->to_date));
      $to = $data->to_date;
    }
    $diff = intval(($to - $from) / (60*60*24*30));
    
    $period_cost = $data->month_cost * $diff;
    if ($data->contract_type == 4){
      $diff = 'N/A';
      $period_cost = $data->month_cost;
      $data->month_cost = 'N/A';
    }
    else{
      $diff = $diff + 1;
    }
    $row[] = $diff;
    $row[] = array('data' => $data->month_cost);
    $row[] = $period_cost;
    $rows[] = $row;
  }
  $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Maintenance Value Table'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
  $html_body .= theme('table', $header, $rows);
  $html_body .= theme('pager', 50);
  $html_body .= "</div>";
  return drupal_get_form('search_maintenance_costs_form') . $html_body;
}

/**
 * maintenance form search elements rendering 
 */
function search_maintenance_costs_form() {
  if (!empty($_GET['date_from'])){
    $date_from = date('Y-m-d', $_GET['date_from']);
  }
  if (!empty($_GET['date_to'])){
    $date_to = date('Y-m-d', $_GET['date_to']);
  }
  $equipment     = $_GET['equipment'];
  $model         = $_GET['model'];
  $contract      = $_GET['contract'];
  $contract_type = $_GET['contract_type'];
  $form['costs'] = array(
    '#title' => 'Search filters',
    '#description' => '<em>'.t('This report shows the money income from the maintenance department, You can choose the income type within a specific dates to calculate the income money during this time').'</em>',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['costs']['equipment'] = array(
    '#type' => 'textfield',
    '#title' => t('Equipment'),
    '#autocomplete_path' => 'equipment/sn/autocomplete/ppm/entry',
    '#default_value' => $equipment,
  );
  $form['costs']['model'] = array(
    '#type'  => 'textfield',
    '#title' => t('Model'),
    '#autocomplete_path' => 'model/autocomplete',
    '#default_value' => $model,
  );
  $form['costs']['contract'] = array(
    '#type'  => 'textfield',
    '#title' => t('Invoice No./ Contract No.'),
    '#autocomplete_path' => 'maintenance/contract/autocomplete',
    '#default_value' => $contract,
  );
  $form['costs']['contract_type'] = array(
    '#type' => 'select',
    '#title' => t('Income type'),
    '#options' => array(
      '' => 'None',
      '1' => 'Under Warranty',
      '2' => 'Under Extended Warranty',
      '3' => 'Under Maintenance Contract',
      '4' => 'Purchase Order',
    ),
    '#default_value' => $contract_type,
  );
  $dateformat = 'Y-m';
  $form['costs']['date_from'] = array(
    '#type' => 'date_popup',
    '#date_format' => $dateformat,
    '#default_value' => $date_from,
    '#title' => t('From date'),
  );
  $form['costs']['date_to'] = array(
    '#type' => 'date_popup',
    '#date_format' => $dateformat,
    '#default_value' => $date_to,
    '#title' => t('To date'),
  );
  $form['costs']['apply'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
  );
  return $form;

}

/**
 * apply submit to filter results of the search
 * related to search_maintenance_costs_form
 */
function search_maintenance_costs_form_submit($form, &$form_state){
  $equipment     = $form_state['values']['equipment'];
  $model         = $form_state['values']['model'];
  $contract      = $form_state['values']['contract'];
  $contract_type = $form_state['values']['contract_type'];
  $date_from     = strtotime($form_state['values']['date_from']);
  $date_to       = strtotime($form_state['values']['date_to']);
  drupal_goto("search/maintenance/costs","equipment=$equipment&model=$model&contract=$contract&contract_type=$contract_type&date_from=$date_from&date_to=$date_to");
}


/**
 * render currency form
 */
function price_of_currency(){
  return drupal_get_form('price_of_currency_form');
}

/**
 * edit currency/price form elements
 */
function price_of_currency_form(){
  $form['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency Info'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['currency']['currency_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency name'),
  );
  $form['currency']['currency_symbol'] = array(
    '#type'  => 'textfield',
    '#title' => t('Currency symbol'),
  );
  $form['currency']['currency_price_in_egp'] = array(
    '#type'  => 'textfield',
    '#title' => t('Currency price in egp'),
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $query = 'SELECT id_of_currency, currency_name, currency_symbol, currency_price_in_egp FROM current_price_of_currency';
  $result = pager_query($query, 50);
  $rows = array();
  $header = array("Currency name", "Currency symbol", "Currency price in egp", "Edit");
  while ($data = db_fetch_object($result)){
    $row = array();
    $edit_path = url('edit/currency/form/'.$data->id_of_currency , array('query' => array('destination' => 'currency/form'))) ;
    $row[] = array('data' => $data->currency_name);
    $row[] = array('data' => $data->currency_symbol);
    $row[] = array('data' => $data->currency_price_in_egp);
    $row[] = array('data' => "<a href='".$edit_path."'>".t('Edit')."</a>");
    $rows[] = $row;
  }
  $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Price of currency Table'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
  $html_body .= theme('table', $header, $rows);
  $html_body .= theme('pager', 50);
  $html_body .= "</div>";
  $form['table'] = array(
    '#value' => $html_body,
  );
  return $form;
}
/**
 * price/currency form submit
 */
function price_of_currency_form_submit($form,&$form_state){
  $currency_name = $form_state['values']['currency_name'];
  $currency_symbol = $form_state['values']['currency_symbol'];
  $currency_price_in_egp = $form_state['values']['currency_price_in_egp'];
  $insert = db_query('INSERT INTO current_price_of_currency (currency_name, currency_symbol, currency_price_in_egp) VALUES("%s", "%s", %d)', $currency_name, $currency_symbol, $currency_price_in_egp);
  drupal_goto("currency/form");
  drupal_set_message('Data have been saved');
}

/**
 * render factors form
 */
function factor(){
  return drupal_get_form('factor_form') . $html_body;
}

/**
 * Factor information form elements
 */
function factor_form(){
  $form['factors'] = array(
    '#type' => 'fieldset',
    '#title' => t('Factor Info'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['factors']['factor_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Factor name'),
  );
  $form['factors']['factor_description'] = array(
    '#type'  => 'textfield',
    '#title' => t('Factor description'),
  );
  $form['factors']['factor_value'] = array(
    '#type'  => 'textfield',
    '#title' => t('Factor value'),
  );
  $form['factors']['factor_type_of_customer'] = array(
    '#type' => 'select',
    '#title' => t('Type of customer'),
    '#options' => array(
      '1' => 'Private',
      '2' => 'Governmental',
    ),
  );
  $form['factors']['factor_taxes'] = array(
    '#type' => 'select',
    '#title' => t('Taxes'),
    '#options' => array(
      '1' => 'Subject',
      '2' => 'Exempt',
    ),
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $query = 'SELECT * FROM factors';
  $result = pager_query($query, 50);
  $rows = array();
  $header = array("Factor name", "Factor description", "Factor value", "Type of customer", "Taxes","Edit");
  while ($data = db_fetch_object($result)){
    $row = array();
    $edit_path = url('edit/factor/form/'.$data->factor_id , array('query' => array('destination' => 'factor/form'))) ;
    $row[] = array('data' => $data->factor_name);
    $row[] = array('data' => $data->factor_description);
    $row[] = array('data' => $data->factor_value);
    if ($data->type_of_customer == 1){
      $row[] = "Private";
    }
    else{
      $row[] = "Governmental";
    }
    if ($data->taxes == 1){
      $row[] = "Subject";
    }
    else{
      $row[] = "Exempt";
    }
    $row[] = array('data' => "<a href='".$edit_path."'>".t('Edit')."</a>");
    $rows[] = $row;
  }
  $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Factors Table'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
  $html_body .= theme('table', $header, $rows);
  $html_body .= theme('pager', 50);
  $html_body .= "</div>";
  $form['table'] = array(
    '#value' => $html_body,
  );
  return $form;
}

/**
 * Factor information form submit 
 */
function factor_form_submit($form,&$form_state){
  $factor_name = $form_state['values']['factor_name'];
  $factor_description = $form_state['values']['factor_description'];
  $factor_value = $form_state['values']['factor_value'];
  $type_of_customer = $form_state['values']['factor_type_of_customer'];
  $taxes = $form_state['values']['factor_taxes'];
  $factor_table_insert = db_query('INSERT INTO factors (factor_name, factor_description, factor_value, type_of_customer, taxes) VALUES("%s", "%s", %s, %d, %d)', $factor_name, $factor_description, $factor_value, $type_of_customer, $taxes);
  drupal_goto("factor/form");
  drupal_set_message('Data have been saved');
}

/**
 * renders form for editing currency/price 
 */
function edit_price_of_currency(){
  return drupal_get_form('edit_price_of_currency_form');
}

/**
 * currency/price edit form elements 
 */
function edit_price_of_currency_form(){
  $id = arg(3);
  $query = db_query('SELECT * FROM current_price_of_currency WHERE id_of_currency = %d', $id);
  while ($result = db_fetch_object($query)){
    $currency_name = $result->currency_name;
    $currency_symbol = $result->currency_symbol;
    $currency_price_in_egp = $result->currency_price_in_egp;
  }
  $form['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency Info'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['currency']['currency_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency name'),
    '#default_value' => $currency_name,
  );
  $form['currency']['currency_symbol'] = array(
    '#type'  => 'textfield',
    '#title' => t('Currency symbol'),
    '#default_value' => $currency_symbol,
  );
  $form['currency']['currency_price_in_egp'] = array(
    '#type'  => 'textfield',
    '#title' => t('Currency price in egp'),
    '#default_value' => $currency_price_in_egp,
  );
  $form['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
}

/**
 * currency/price edit form submit
 */
function edit_price_of_currency_form_submit($form, &$form_state){
  $id = arg(3);
  $currency_name = $form_state['values']['currency_name'];
  $currency_symbol = $form_state['values']['currency_symbol'];
  $currency_price_in_egp = $form_state['values']['currency_price_in_egp'];
  $insert = db_query('UPDATE current_price_of_currency SET currency_name = "%s", currency_symbol = "%s", currency_price_in_egp = %d WHERE id_of_currency = %d',$currency_name, $currency_symbol, $currency_price_in_egp, $id);
  drupal_goto("currency/form");
}

/**
 * render editing for spesific factor
 */
function edit_factor(){
  return drupal_get_form('edit_factor_form');
}

/**
 * edit spesific factor form 
 */
function edit_factor_form(){
  $id = arg(3);
  $query = db_query('SELECT * FROM factors WHERE factor_id = %d', $id);
  while ($result = db_fetch_object($query)){
    $factor_name = $result->factor_name;
    $factor_description = $result->factor_description;
    $factor_value = $result->factor_value;
    $type_of_customer = $result->type_of_customer;
    $taxes = $result->taxes;
  }
  $form['factors'] = array(
    '#type' => 'fieldset',
    '#title' => t('Factor Info'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['factors']['factor_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Factor name'),
    '#default_value' => $factor_name,
  );
  $form['factors']['factor_description'] = array(
    '#type'  => 'textfield',
    '#title' => t('Factor description'),
    '#default_value' => $factor_description,
  );
  $form['factors']['factor_value'] = array(
    '#type'  => 'textfield',
    '#title' => t('Factor value'),
    '#default_value' => $factor_value,
  );
  $form['factors']['factor_type_of_customer'] = array(
    '#type' => 'select',
    '#title' => t('Type of customer'),
    '#options' => array(
      '1' => 'Private',
      '2' => 'Governmental',
    ),
    '#default_value' => $type_of_customer,
  );
  $form['factors']['factor_taxes'] = array(
    '#type' => 'select',
    '#title' => t('Taxes'),
    '#options' => array(
      '1' => 'Subject',
      '2' => 'Exempt',
    ),
    '#default_value' => $taxes,
  );
  $form['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
}

/**
 * spesific factor form submit 
 */
function edit_factor_form_submit($form, &$form_state){

 // print_r($form_state['values']); exit();
  $id = arg(3);
  $factor_name = $form_state['values']['factor_name'];
  $factor_description = $form_state['values']['factor_description'];
  $factor_value = $form_state['values']['factor_value'];
  $type_of_customer = $form_state['values']['factor_type_of_customer'];
  $taxes = $form_state['values']['factor_taxes'];
  $insert = db_query('UPDATE factors SET factor_name = "%s", factor_description = "%s", factor_value = %s, type_of_customer = %d, taxes = %d WHERE factor_id = %d',$factor_name, $factor_description, $factor_value , $type_of_customer, $taxes, $id);
  drupal_set_message('Data have been updated');
  drupal_goto("factor/form");
}

/**
 * Returns the count of item requested for quotations 
 * that have not been issued PO, for specific part catalog 
 */
function get_items_count_quotation_for_part_catalog($part_nid){
  $query = db_query('SELECT items_count.field_quotaion_items_count_value items_count FROM content_field_quotation_item_code itemcode INNER JOIN content_field_quotaion_items_count items_count ON itemcode.nid = items_count.nid left join content_type_importcode po on (items_count.nid = po.field_import_code_quotation_id_nid) LEFT JOIN content_field_quotation_offer_type offer_type on (offer_type.nid = itemcode.nid AND offer_type.delta = itemcode.delta) WHERE itemcode.delta = items_count.delta AND po.nid is null AND itemcode.field_quotation_item_code_nid = %d  AND offer_type.field_quotation_offer_type_value = 1', $part_nid);
  while ($result = db_fetch_object($query)){
    $count += $result->items_count;
  }
  return $count;
}


/**
 * Returns count of PO for an opened job/call for
 * specific part catalog
 */
function get_opened_po_for_part_catalog($part_nid){
  $query = db_query('SELECT qty_received.field_importcode_qty_received_value qty_received_count, itemcode.field_importcode_item_code_nid items_nid, items_count.field_importcode_items_count_value items_count_value FROM content_field_importcode_item_code itemcode inner join content_field_importcode_items_count items_count on items_count.nid = itemcode.nid inner join content_type_importcode importcode on importcode.nid = itemcode.nid inner join content_field_job_status job_status on job_status.nid =importcode.field_importcode_job_nid INNER JOIN content_field_importcode_qty_received qty_received ON qty_received.nid = items_count.nid WHERE items_count.delta = qty_received.delta AND itemcode.delta = items_count.delta AND job_status.field_job_status_value < 6 AND itemcode.field_importcode_item_code_nid = %d', $part_nid);
  while ($result = db_fetch_object($query)){
    $qty_requested_count += $result->items_count_value;
    $qty_received_count  += $result->qty_received_count;
  }
  $count = $qty_requested_count - $qty_received_count;
  return $count;
}


/**
 * autocomplete part catalog number field
 */
function autocomlete_part_catalog_number($string){
  $matches = array();
  $query = db_query_range("SELECT node.title title FROM {content_type_part_catalog} INNER JOIN node USING(nid) WHERE LOWER(node.title) LIKE LOWER('%s%')", $string, 0, 20);
  while ($data = db_fetch_object($query)) {
    $matches[$data->title] = check_plain($data->title);
  }
  print drupal_to_js($matches);
  exit();
}


/**
 * autocomplete company name field
 */
function return_autocomplete_company_name($string){
  $matches = array();
  $query = db_query_range("SELECT title FROM node WHERE type = 'company' AND LOWER(title) LIKE LOWER('%s%')", $string, 0, 20);
  while ($data = db_fetch_object($query)) {
    $matches[$data->title] = check_plain($data->title);
  }
  print drupal_to_js($matches);
  exit();
}


/**
 * part catalog re-question table
 */
function part_catalog_requestion(){
  $and = "";
  $part_number = $_GET['part_number'];
  $model = $_GET['model'];
  $client = $_GET['client'];
  $company_name= $_GET['company_name'];
  if (!empty($part_number)){
    $and .= " AND node.title = '{$part_number}'";
  }
  if (!empty($model)){
    $and .= " AND term_data.name = '{$model}'";
  }
  if (!empty($client)){
    $and .= " AND client_node.title = '{$client}'";
  }
  if (!empty($company_name)){
    $and .= " AND node_company.title = '{$company_name}'";
  }
  $query = 'select part_catalog.field_part_min_quantity_value standard_value, term_data.name item_model, client_node.title client_name, part_catalog.field_part_qty_on_hand_value current_stock, part_catalog.field_part_data_part_name_e_value description, node.title part_number, q.nid quotation_nid, ic.field_quotaion_items_count_value as q_count, c.field_quotation_item_code_nid as item_nid from content_type_quotation q inner join content_field_quotation_item_code c on (c.nid = q.nid) inner join content_field_quotaion_items_count ic on (ic.nid = q.nid) INNER JOIN node ON node.nid = c.field_quotation_item_code_nid INNER JOIN content_type_part_catalog part_catalog ON part_catalog.nid = c.field_quotation_item_code_nid INNER JOIN content_field_job_client job_client ON job_client.nid = q.field_quotation_job_nid INNER JOIN node client_node ON client_node.nid = job_client.field_job_client_nid INNER JOIN content_field_type_l2 model ON model.nid = part_catalog.nid LEFT join term_data on model.field_type_l2_value = term_data.tid LEFT JOIN node node_company ON part_catalog.field_part_company_sup_nid = node_company.nid where node.title != ""' . $and;
  $result = pager_query($query, 50);
  $rows = array();
  $header = array(t("Part number"), t("Description"), t("Current stock"), t("Standard stock"), t("Model"), t("Number of SO(s)"), t("Number of Quotations without SO"));
  while ($data = db_fetch_object($result)){
    $row = array();
    $part_path = url('node/' . $data->item_nid);
    $row[] = "<a href='". $part_path . "'>" . $data->part_number . "</a>";
    if (empty($data->description)){
      $row[] = "N/A";
    }
    else{
      $row[] = $data->description;
    }
    if (empty($data->current_stock)){
      $row[] = "N/A";
    }
    else{
      $row[] = $data->current_stock;
    }
    if (empty($data->standard_value)){
      $row[] = "N/A";
    }
    else{
      $row[] = $data->standard_value;
    }

    if (empty($data->item_model)){
      $row[] = "N/A";
    }
    else{
      $row[] = $data->item_model;
    }
    $item_count = get_opened_po_for_part_catalog($data->item_nid);
    if (empty($item_count)){
      $row[] = "N/A";
    }
    else{
      $part_transaction_path = url('store_transactions', array('query' => array('title' => $data->part_number)));
      $row[] = $item_count;//"<a href='".$part_transaction_path ."' >". $item_count."</a>";
    }
    $part_transaction_path_for_quotation = url('store_transactions', array('query' => array('title' => $data->part_number)));
    $quotation_items_count = get_items_count_quotation_for_part_catalog($data->item_nid);
    if (empty($quotation_items_count)){
      $row[] = "N/A";
    }
    else{
      $part_transaction_path = url('store_transactions', array('query' => array('title' => $data->part_number)));
      $row[] = $quotation_items_count;//"<a href='".$part_transaction_path ."' >". $quotation_items_count."</a>";
    }
    $rows[$data->item_nid] = $row;
  }
  $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Part catalog requisition report'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
  $html_body .= theme('table', $header, $rows);
  $html_body .= theme('pager', 50);
  $html_body .= "</div>";

  return drupal_get_form('part_catalog_requestion_form') . $html_body;
}

/**
 * part catalog re-question filters
 * related to part_catalog_requestion
 */
function part_catalog_requestion_form(){
  $part_number = $_GET['part_number'];
  $model = $_GET['model'];
  $client = $_GET['client'];
  $company_name= $_GET['company_name'];
  $form['part']['supplier_name'] = array(
    '#type'  => 'textfield',
    '#title' => t('Supplier Company'),
    '#autocomplete_path' => 'return/company/name/autocomplete',
    '#default_value' => $company_name,
  );
  $form['part']['number'] = array(
    '#type'  => 'textfield',
    '#title' => t('Part Number'),
    '#default_value' => $part_number,
    '#autocomplete_path' => 'autocomplete/part/catalog/number',
  );
  $form['part']['model'] = array(
    '#type'  => 'textfield',
    '#title' => t('Model'),
    '#autocomplete_path' => 'model/autocomplete/ppm/cal/entry',
    '#default_value' => $model,
  );
  $form['part']['client'] = array(
    '#type'  => 'textfield',
    '#title' => t('Client'),
    '#autocomplete_path' => 'client/autocomplete',
    '#default_value' => $client,
  );
  $form['part']['apply'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
  );
  return $form;
}

/**
 * apply submit to filter results of the search
 * related to part_catalog_requestion_form
 */
function part_catalog_requestion_form_submit($form, &$form_state){
  $part_number = $form_state['values']['number'];
  $client = $form_state['values']['client'];
  $model = $form_state['values']['model'];
  $company_name = $form_state['values']['supplier_name'];
  drupal_goto("requestion/part/catalog/report","part_number=$part_number&model=$model&client=$client&company_name=$company_name");
}

/**
 * render a form to edit work flow status'
 */
function edit_workflow_status(){
  return drupal_get_form('edit_workflow_status_form');
}

/**
 * edit work flow status form elements 
 */
function edit_workflow_status_form(){
  $status = array();
  $query = db_query('SELECT id, name FROM workflow_status');
  while ($result = db_fetch_object($query)){
    $status[$result->id] = $result->name;
  }
  $form['workflow']['status'] = array(
    '#type' => 'select',
    '#title' => t('Workflow Status'),
    '#options' => array('' => 'None') + $status,
  );
  $form['workflow']['change'] = array(
    '#type' => 'textfield',
    '#title' => t('Change the status to'),
  );
  $form['status']['apply'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * edit work flow status form submit
 */
function edit_workflow_status_form_submit($form, &$form_state){
  $status = $form_state['values']['status'];
  $change = $form_state['values']['change'];
  $query = db_query('UPDATE workflow_status SET name = "%s" WHERE id = %d', $change, $status);
  drupal_set_message('Status has been updated sucessfully ...');
}

function return_employees_calendar_item_ajax_edit(){
  $res = db_query("SELECT node.uid uid, node.title name FROM node INNER JOIN content_type_hospital_employee employee using(nid)");
  while ($row = db_fetch_array($res)) {
    $result[] = array(
      'uid' => $row['uid'],
      'name' => $row['name'],
    );
  }
  echo json_encode($result);
}


function _update_calendar_item_info_ajax_edit(){
  global $user;
  $nid = arg(2);
  $expected_date = arg(3);
  $assigned_employee = arg(4);
  $node = node_load($nid);
  $type = $node->type;
  if ($type == 'pentry'){
    if ($expected_date != '_'){
      $node->field_pentry_date[0]['value'] = $expected_date;
    }
    if ($assigned_employee != '_'){
      if (!empty($user->roles[FIELD_ENGINEER]) || !empty($user->roles[TECHNICIAN]) || $node->field_pentry_hospital_person[0]['uid'] == $user->uid) { 
        echo t("You are not allowed to assign employee other than you"); exit();
      }
      else {
        // it means that the manager is updating with another employee
        $node->field_pentry_hospital_person[0]['uid'] = $assigned_employee;
      }

    }
  }
  if ($type == 'centry'){
    if ($expected_date != '_'){
      $node->field_centry_date[0]['value'] = $expected_date;
    }
    if ($assigned_employee != '_'){
      if (!empty($user->roles[FIELD_ENGINEER]) || !empty($user->roles[TECHNICIAN]) || $node->field_hospital_person[0]['uid'] == $user->uid) {
        echo t("You are not allowed to assign employee other than you"); exit();
      }
      else {
        $node->field_hospital_person[0]['uid'] = $assigned_employee;
      }
    }
  }
  if ($type == 'job'){
    if ($expected_date != '_'){
      $node->field_job_expected_date_of_visit[0]['value'] = $expected_date;
    }
    if ($assigned_employee != '_'){
      if (!empty($user->roles[FIELD_ENGINEER]) || !empty($user->roles[TECHNICIAN]) || $node->field_job_assigned_user[0]['uid'] == $user->uid) { 
        echo t("You are not allowed to assign employee other than you"); exit();
      }
      else {
        $node->field_job_assigned_user[0]['uid'] = $assigned_employee;
      }
    }
  }
  node_save($node);
  echo 1;
}

/**
 * render upload client form 
 */
function upload_client(){
  return drupal_get_form('upload_client_form');
}

/**
 * render upload model form
 */
function upload_model(){
  return drupal_get_form('upload_model_form');
}

/**
 * render upload call form
 */
function upload_call(){
  return drupal_get_form('upload_call_form');
}

/**
 * upload call form element
 */
function upload_call_form(){

  $form['guidelines'] = array(
    '#value' => '
                <font color="green">Guidelines to import file into the system</font>
                 <br>1- Convert file to csv file
                 <br>2- Prepare file to be uploaded into the system
                   <br>(a) Insert node like equipment/brand/model ... by id or nid so you have to change the name by id or nid
                   <br>(b) If equipment/brand or model isn\'t in the system, so you have to add the requested node then get node nid to be converted into the CSV file to match the existing data in the system
                   <br>(c) Another values in the file to be set with its values if the field to be set is textfield, no problem .. if the field to be set is select list, you need to convert the statments or the values to numbers to match the select list values in the system
                 <br>3- Take backup from database before the operation of import the file
                 <br>4- After the file has been ready, first review the sample data to be imported in the system, if everythings is fine upload the file again and hit import.!!
                 <br><font color="green">Sample structure</font>
                 <table border=1px style=background-color:#eee><tr>
                 <td>Call Id</td><td>Status of job</td><td>Equipment S.N</td><td>Complain</td><td>Date of call</td><td>End of work</td></tr></table>
                 <br><span style="color:red"><b>Notice : The columns order is very important</b></span>',
  );
  $form['upload_call'] = array(
    '#title' => 'Upload Calls',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['upload_call']['upload'] = array(
    '#type' => 'file',
    '#description' => t('This form allow you to import calls into system'),
  );
  $form['upload_call']['file_header'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check this if the file contains the header'),
  );
  $form['upload_call']['bypass_validation'] = array(
    '#type' => 'checkbox',
    '#title' => t('By pass validation'),
  );
  $form['upload_call']['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimiter'),
    '#options' => array(
      ';' => 'Semicolon : ";"',
      ',' => 'Comma : ","',
    ),
    '#required' => 'true',
  );

  $form['upload_call']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import',
  );
  $form['upload_call']['preview'] = array(
    '#type' => 'submit',
    '#value' => 'Preview',
  );
  if($_GET['upload_status'] == "preview") {
    $header = array("Call id", "Status of job", "Equipment S.N", "Complain", "Date of call", "End of work");
    session_start();
    $rows = $_SESSION['sample_calls_upload'];
  }
  // display table of sapmle data in case of preview status
  if ($_GET['upload_status'] == 'preview') {
    $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Sample Calls from upload'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
    $html_body .= theme('table', $header, $rows);
    $html_body .= "</div></div>";
    $form['preview'] = array(
      '#value' => $html_body,
    );
  }

  return $form;
}

/**
 * upload call form submit button
 */
function upload_call_form_submit($form, &$form_state){
  global $hospital_nid;
  $validators = array();
  $row = array();
  $i = 0;
  $row_num = 0;
  $file_header = 0;
  $file_header = $form_state['values']['file_header'];
  $file = file_save_upload('upload', $validators, file_directory_path());
  $csv_uri = $file->filepath;        //file URL
  if (empty($csv_uri)){
    drupal_set_message('No file is chosen');
    return;
  }
  $handle = fopen($csv_uri, 'r');   //open file -> read only
  while(! feof($handle)){
    $delimiter = $form_state['values']['delimiter'];
    $row = fgetcsv($handle, 1000, $delimiter);
    if (($file_header == 1 && $i !=0) || $file_header == 0){
      file_set_status($file, FILE_STATUS_PERMANENT);
      $node = new stdClass();
      $node->type = 'job';
      $node->uid = 1;
      $node->created = time();
      $node->field_job_hospital[0]['nid'] = $hospital_nid;
      $node->title = $row[0];
      $node->field_job_status[0]['value'] = $row[1];
      $node->field_job_medical_equip[0]['nid'] = $row[2];
      $node->field_nature_of_complaint[0]['value'] = $row[3];
      $node->field_requested_date[0]['value'] = strtotime($row[4]);
      $node->field_end_of_work[0]['value'] = strtotime($row[5]);
      if (variable_get('show_second_sn_in_call', 0) == 1){
        $node->field_call_second_sn[0]['value'] = $row[6];
      }
      if ($form_state['clicked_button']['#id'] =='edit-submit'){
        if (!empty($row[0])){
          _cis_node_save($node);
        }
      }
      if ($i < 20) {
        $calls_sample_array[] = $row;
      }
      $bypass_validation = $form_state['values']['bypass_validation'];
      if (!empty($row[2]) && $bypass_validation != 1){
        if ( !is_numeric($row[1]) || !is_numeric($row[2])) {
           $row_num = $i+1;
           form_set_error("", "There are none numeric data in the the following row number $row_num ");
        }
      }
    }
    $i++;
  }
  if ($form_state['clicked_button']['#id'] =='edit-submit'){
    drupal_set_message($i-1 . t(' Calls have been imported into the system'));
  }
  else if  ($form_state['clicked_button']['#id'] =='edit-preview'){
    if ($row_num == 0){
      drupal_set_message(t('No problem, the sheet is ready to be imported'));
    }
    session_start();
    $_SESSION['sample_calls_upload'] = $calls_sample_array;
    drupal_goto('upload/calls','upload_status=preview');
  }
}

/**
 * render upload equipment form
 */
function upload_equipment(){
  global $cmms_type;
  if ($cmms_type == 0 || $cmms_type == 1){
    return drupal_get_form('upload_equipment_form');
  }
  if ($cmms_type == 2 || $cmms_type == 3){
    return drupal_get_form('upload_equipment_for_company_form');
  }
}

/**
 * upload model form element
 */
function upload_model_form(){
  if ( $_GET['sheet_ready'] != 1 && ($_GET['upload_status'] == "preview" || $_GET['save_brands_models'] == 1)){
    $cancel = l('Cancel', 'upload/models');
    $form['brand_model_save_btn'] = array(
      '#type' => 'submit',
      '#value' => 'Save Brands & Models',
    );
    $form['cancel'] = array('#value' => $cancel);
  }
  $form['guidelines'] = array(
    '#value' => '
                <font color="green"><br><b>Guidelines to import file into the system</b></font>
                 <br>1- Convert file to csv file
                 <br>2- Prepare file to be uploaded into the system
                   <br>(a) Insert node like equipment/brand/model ... by id or nid so you have to change the name by id or nid
                   <br>(b) If equipment/brand or model isn\'t in the system, so you have to add the requested node then get node nid to be converted into the CSV file to match the existing data in the system
                   <br>(c) Another values in the file to be set with its values if the field to be set is textfield, no problem .. if the field to be set is select list, you need to convert the statments or the values to numbers to match the select list values in the system
                 <br>3- Take backup from database before the operation of import the file
                 <br>4- After the file has been ready, first review the sample data to be imported in the system, if everythings is fine upload the file again and hit import.!!
                 <br><font color="green"><b>Sample structure</b></font>
                 <table border=1px style=background-color:#eee><tr>
                 <td>Equipment name</td><td>Brand</td><td>Model</td><td>Power source</td><td>Reference no.</td><td>Company agency</td><td>Warranty period</td><td>PPM tasks</td><td>Calibration tasks</td><td>Stop production date</td><td>End of support date</td><td>Make</td></tr></table>
                 <br><span style="color:red"><b>Notice : The columns order is very important</b></span>',
  );
  $form['upload_model'] = array(
    '#title' => 'Upload Models',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['upload_model']['upload'] = array(
    '#type' => 'file',
    '#description' => t('This form allow you to import models into system'),
  );
  $form['upload_model']['file_header'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check this if the file contains the header'),
  );
  $form['upload_model']['bypass_validation'] = array(
    '#type' => 'checkbox',
    '#title' => t('By pass validation'),
  );
  $form['upload_model']['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimiter'),
    '#options' => array(
      ';' => 'Semicolon : ";"',
      ',' => 'Comma : ","',
    ),
    '#required' => 'true',
  );

  $form['upload_model']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import',
  );
  $form['upload_model']['preview'] = array(
    '#type' => 'submit',
    '#value' => 'Preview',
  );
  if($_GET['upload_status'] == "preview") {
    $header = array("Equipment name", "Brand", "Model", "Power source", "Reference no.", "Company agency", "Warranty period", "PPM tasks", "Calibration tasks", "Stop production date", "End of support date", "Make");
    session_start();
    $rows = $_SESSION['sample_models_upload'];
  }
  // display table of sapmle data in case of preview status
  if ($_GET['upload_status'] == 'preview') {
    $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Sample Models from upload'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
    $html_body .= theme('table', $header, $rows);
    $html_body .= "</div></div>";
    $form['preview'] = array(
      '#value' => $html_body,
    );
  }

  return $form;
}

/**
 * upload model form submit button
 */
function upload_model_form_submit($form, &$form_state){
  $models_arr = array();
  $brands_arr = array();
  $query = 'SELECT term_data.name as brand_name, h1.tid as tid1, h2.tid as tid2 , h3.tid as tid3 FROM term_hierarchy h1 left join term_hierarchy h2 on h1.parent = h2.tid left join term_hierarchy h3 on h2.parent = h3.tid inner join term_data on h1.tid = term_data.tid where vid = 1 and  h2.tid is NULL';
  $result = db_query($query);
  while ($row = db_fetch_object($result)){
    $brands_arr[$row->brand_name] = $row->tid1;
  }
  $query = 'SELECT term_data.name as name, h1.tid as tid1, h2.tid as tid2 , h3.tid as tid3 FROM term_hierarchy h1 left join term_hierarchy h2 on h1.parent = h2.tid left join term_hierarchy h3 on h2.parent = h3.tid inner join term_data on h1.tid = term_data.tid where vid = 1 and  h2.tid > 0';
  $result = db_query($query);
  while ($row = db_fetch_object($result)){
    $models_arr[$row->name] = $row->tid1;
  }
  $validators = array();
  $row = array();
  $brands_not_exist_arr = array();
  $models_not_exist_arr = array();
  $i = 0;
  $row_num = 0;
  $x = 0;
  $file_header = 0;
  $brand_model_arr_to_save = 0;
  $file_header = $form_state['values']['file_header'];
  $file = file_save_upload('upload', $validators, file_directory_path());
  $csv_uri = $file->filepath;        //file URL
  if (empty($csv_uri)){
    drupal_set_message('No file is chosen');
    return;
  }
  $handle = fopen($csv_uri, 'r');   //open file -> read only
  while(! feof($handle)){
    $delimiter = $form_state['values']['delimiter'];
    $row = fgetcsv($handle, 1000, $delimiter);
    if (($file_header == 1 && $i !=0) || $file_header == 0){
      file_set_status($file, FILE_STATUS_PERMANENT);
      $node = new stdClass();
      $node->type = 'item_data';
      $node->uid = 1;
      $node->created = time();
      $node->title = $row[4];
      $equipment_name_arr = collect_equipment_name();
      $node->field_equ_name_english[0]['value'] = $equipment_name_arr['ar'][$row[0]];
      $node->field_equipment_name[0]['value'] = $equipment_name_arr['ar'][$row[0]];
      $node->field_type_l1[0]['value'] = $brands_arr[$row[1]];
      $node->field_type_l2[0]['value'] = $models_arr[$row[2]];
      $node->field_item_power_source[0]['value'] = $row[3];
      $node->field_item_reference_no[0]['value'] = $row[4];
      $node->field_item_company_sup[0]['nid'] = $row[5];
      $node->field_item_warranty_period[0]['value'] = $row[6];
      $node->field_item_ppm_tasks[0]['value'] = $row[7];
      $node->field_item_cal_tasks[0]['value'] = $row[8];
      $node->field_item_stop_production_date[0]['value'] = $row[9];
      $node->field_item_end_support_date[0]['value'] = $row[10];
      $node->field_make_text[0]['value'] = $row[11];
      if ($form_state['clicked_button']['#id'] =='edit-submit'){
        if (!empty($row[4])){
          _cis_node_save($node);
          $x++;
        }
      }
      if ($i < 50) {
        $models_sample_array[] = $row;
      }
      if (empty($brands_arr[$row[1]]) && !empty($row[1])){
        $brands_not_exist_arr[$row[1]] = $row[1];
        $brand_model_arr_to_save++;
        if ( $form_state['clicked_button']['#id'] == 'edit-preview'){
          drupal_set_message(t('Brand name : ') . $row[1] . t(' is not exist in the system'));
        }
      }
      if (empty($models_arr[$row[2]]) && !empty($row[2])){
        $models_not_exist_arr[$row[2]] = $row[1];
        $brand_model_arr_to_save++;
        if ( $form_state['clicked_button']['#id'] == 'edit-preview'){
          drupal_set_message(t('Model name : ') . $row[2] . t(' is not exist in the system'));
        }
      }
      $bypass_validation = $form_state['values']['bypass_validation'];
      if (!empty($row[2]) && $bypass_validation != 1){
        if ( !is_numeric($row[0]) || !is_numeric($row[5]) || !is_numeric($row[6]) || !is_numeric($row[11])) {
           $row_num = $i+1;
           form_set_error("", "There are none numeric data in the the following row number $row_num ");
        }
      }
    }
    $i++;
  }
  if ($form_state['clicked_button']['#id'] =='edit-submit'){
    drupal_set_message($x . t(' Models have been imported into the system'));
  }
  else if  ($form_state['clicked_button']['#id'] =='edit-preview'){
    if ( $row_num == 0 && $brand_model_arr_to_save == 0){
      drupal_set_message(t('No problem, the sheet is ready to be imported'));
      drupal_goto('upload/models','upload_status=preview&sheet_ready=1');
    }
    session_start();
    $_SESSION['sample_models_upload'] = $models_sample_array;
    drupal_goto('upload/models','upload_status=preview');
  }
  else if ($form_state['clicked_button']['#id'] == 'edit-brand-model-save-btn' && $_GET['upload_status'] == 'preview'){
    foreach ($brands_not_exist_arr as $key => $value){
      $taxonomy_save_brand_term = array (
        'vid' => 1, // Voacabulary ID
        'name' => $value, // Term Name
        'parent' => '',
      );
      taxonomy_save_term($taxonomy_save_brand_term);
      drupal_set_message(t('Brand name : ') . $value . t(' has been saved into the system'));
    }
    foreach ($models_not_exist_arr as $key => $value){
      $brand_tid = db_result(db_query('SELECT tid FROM term_data INNER JOIN term_hierarchy USING (tid) WHERE parent = 0 and name = "%s"', $value));
      $taxonomy_save_model_term = array (
        'vid' => 1, // Voacabulary ID
        'name' => $key, // Term Name
        'parent' => $brand_tid,
      );
      taxonomy_save_term($taxonomy_save_model_term);
      drupal_set_message(t('Model name : ') . $key . t('has been saved into the system under parent ') . $value);
    }
    drupal_goto('upload/models','save_brands_models=1');
  }
}

/**
 * upload client form element
 */
function upload_client_form(){

  $form['guidelines'] = array(
    '#value' => '
                <font color="green">Guidelines to import file into the system</font>
                 <br>1- Convert file to csv file
                 <br>2- Prepare file to be uploaded into the system
                   <br>(a) Insert node like equipment/brand/model ... by id or nid so you have to change the name by id or nid
                   <br>(b) If equipment/brand or model isn\'t in the system, so you have to add the requested node then get node nid to be converted into the CSV file to match the existing data in the system
                   <br>(c) Another values in the file to be set with its values if the field to be set is textfield, no problem .. if the field to be set is select list, you need to convert the statments or the values to numbers to match the select list values in the system
                 <br>3- Take backup from database before the operation of import the file
                 <br>4- After the file has been ready, first review the sample data to be imported in the system, if everythings is fine upload the file again and hit import.!!
                 <br><font color="green">Sample structure</font>
                 <table border=1px style=background-color:#eee><tr>
                 <td>Client name (e)</td><td>Client code</td><td>Client arabic Name</td><td>Type of customer</td><td>Collection Rep.</td><td>Group</td><td>Sales Rep.</td><td>City</td><td>Last collection period 1</td><td>Last collection period 2</td><td>Credit limit</td><td>Sub Rep. for new Rev. centers</td><td>Taxes</td><td>Categorization</td><td>Initial</td></tr></table>
                 <br><span style="color:red"><b>Notice : The columns order is very important</b></span>',
  );
  $form['upload_client'] = array(
    '#title' => 'Import Clients',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['upload_client']['upload'] = array(
    '#type' => 'file',
    '#description' => t('This form allow you to import clients into system'),
  );
  $form['upload_client']['file_header'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check this if the file contains the header'),
  );
  $form['upload_client']['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimiter'),
    '#options' => array(
      ';' => 'Semicolon : ";"',
      ',' => 'Comma : ","',
    ),
    '#required' => 'true',
  );

  $form['upload_client']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import',
  );
  $form['upload_client']['preview'] = array(
    '#type' => 'submit',
    '#value' => 'Preview',
  );
  if($_GET['upload_status'] == "preview") {
    $header = array("Client name (e)", "Client code", "Client arabic Name", "Type of customer", "Collection Rep.", "Group", "Sales Rep.", "City", "Last collection period 1", "Last collection period 2", "Credit limit", "Sub Rep. for new Rev. centers", "Taxes", "Categorization", "Initial");
    session_start();
    $rows = $_SESSION['sample_clients_upload'];
  }
  // display table of sapmle data in case of preview status
  if ($_GET['upload_status'] == 'preview') {
    $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Sample Clients from upload'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
    $html_body .= theme('table', $header, $rows);
    $html_body .= "</div></div>";
    $form['preview'] = array(
      '#value' => $html_body,
    );
  }

  return $form;
}

/**
 * upload client form submit button
 */
function upload_client_form_submit($form, &$form_state){
  $validators = array();
  $row = array();
  $i = 0;
  $file_header = 0; // this means that the file has been uploaded without header
  $file_header = $form_state['values']['file_header'];
  $file = file_save_upload('upload', $validators, file_directory_path());
  $csv_uri = $file->filepath;        //file URL
  if (empty($csv_uri)){
    drupal_set_message('No file has been chosen');
    return;
  }
  $handle = fopen($csv_uri, 'r');   //open file -> read only
  while(!feof($handle)){
    $delimiter = $form_state['values']['delimiter'];
    $row = fgetcsv($handle, 1000, $delimiter);
    if (empty($row[0])) {  // this check has been made to remove empty row has been added to the system
      continue;
    }
    if (($file_header == 1 && $i !=0) || $file_header == 0){
      file_set_status($file, FILE_STATUS_PERMANENT);
      $node = new stdClass();
      $node->type = 'client';
      if (!empty($row[0]))
        $row[0] = iconv("windows-1256" , "utf8" , $row[0]);  // to fix arabic encoding UTF-8
      if (!empty($row[2]))
        $row[2] = iconv("windows-1256" , "utf8" , $row[2]);  // to fix arabic encoding UTF-8
      if (!empty($row[4]))
        $row[4] = iconv("windows-1256" , "utf8" , $row[4]);  // to fix arabic encoding UTF-8
      if (!empty($row[7]))
        $row[7] = iconv("windows-1256" , "utf8" , $row[7]);  // to fix arabic encoding UTF-8
      $node->uid = 1;
      $node->created = time();
      $node->title = $row[0];
      $node->field_client_code[0]['value'] = $row[1];
      $node->field_client_name_in_arabic[0]['value'] = $row[2];
      $node->field_client_type_of_customer[0]['value'] = $row[3];
      $node->field_client_invoice_collect_rep[0]['value'] = $row[4];
      $node->field_client_group[0]['value'] = $row[5];
      $node->field_client_sales_rep[0]['value'] = $row[6];
      $node->field_cient_city[0]['value'] = $row[7];
      $node->field_client_last_coll_per_1[0]['value'] = $row[8];
      $node->field_client_last_coll_per_2[0]['value'] = $row[9];
      $node->field_client_max_credit_limit[0]['value'] = $row[10];
      $node->field_client_sales_rep_new_rev_c[0]['value'] = $row[11];
      $node->field_client_taxes[0]['value'] = $row[12];
      $node->field_client_categorization[0]['value'] = $row[13];
      $node->field_client_initial_letter[0]['value'] = $row[14];
      if ($form_state['clicked_button']['#id'] =='edit-submit'){
        if (!empty($node->title)){
          _cis_node_save($node);
        }
      }
      if ($i < 15 && !empty($row[0])) {  // Sample data to be displayed to the user to confirm  50
        $clients_sample_array[] = $row;
      }
      if ( !is_numeric($row[1]) || !is_numeric($row[3]) || !is_numeric($row[5]) || !is_numeric($row[8]) || !is_numeric($row[9]) || !is_numeric($row[12])) {
         $row_num = $i+1;
         form_set_error("", "There are none numeric data in the the following row number $row_num ");
      }
    }
    $i++;
  }
  if ($form_state['clicked_button']['#id'] =='edit-submit'){
    drupal_set_message($i-1 . t(' Clients have been imported into the system'));
  }
  else if  ($form_state['clicked_button']['#id'] =='edit-preview'){
    session_start();
    $_SESSION['sample_clients_upload'] = $clients_sample_array;
    drupal_goto('upload/clients','upload_status=preview');
  }
}

/**
 * upload equipment form element
 * specific for company
 */
function upload_equipment_for_company_form(){
  $form['guidelines'] = array(
    '#value' => '
                <font color="green">Guidelines to import file into the system</font>
                 <br>1- Convert file to csv file
                 <br>2- Prepare file to be uploaded into the system
                   <br>(a) Insert node like equipment/brand/model ... by id or nid so you have to change the name by id or nid
                   <br>(b) If equipment/brand or model isn\'t in the system, so you have to add the requested node then get node nid to be converted into the CSV file to match the existing data in the system
                   <br>(c) Another values in the file to be set with its values if the field to be set is textfield, no problem .. if the field to be set is select list, you need to convert the statments or the values to numbers to match the select list values in the system
                 <br>3- Take backup from database before the operation of import the file
                 <br>4- After the file has been ready, first review the sample data to be imported in the system, if everythings is fine upload the file again and hit import.!!
                 <br><font color="green">Sample structure</font>
                 <table border=1px style=background-color:#eee><tr>
                 <td>Client code</td><td>Model code</td><td>Serial Number</td><td>Operation date</td><td>Warranty months</td><td>PPM Time Period</td></tr></table>
                 <br><span style="color:red"><b>Notice : The columns order is very important</b></span>',
  );
  $form['upload_equ'] = array(
    '#title' => 'Upload Equipment',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['upload_equ']['upload'] = array(
    '#type' => 'file',
    '#description' => t('This form allow you to import equipment into system'),
  );
  $form['upload_equ']['file_header'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check this if the file contains the header'),
  );
  $form['upload_equ']['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimiter'),
    '#options' => array(
      ';' => 'Semicolon : ";"',
      ',' => 'Comma : ","',
    ),
    '#required' => 'true',
  );
  $form['upload_equ']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import',
  );
  $form['upload_equ']['preview'] = array(
    '#type' => 'submit',
    '#value' => 'Preview',
  );
  if($_GET['upload_status'] == "preview") {
    $header = array("Client code", "Model code", "Serial Number", "Operation date", "Warranty months", "PPM Time Period");
    session_start();
    $rows = $_SESSION['sample_equipment_upload_for_company'];
  }
  // display table of sapmle data in case of preview status
  if ($_GET['upload_status'] == 'preview') {
    $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Sample Equipment Upload Table'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
    $html_body .= theme('table', $header, $rows);
    $html_body .= "</div></div>";
    $form['preview'] = array(
      '#value' => $html_body,
    );
  }
  return $form;
}

/**
 * upload equipment submit
 * specific for company
 */
function upload_equipment_for_company_form_submit($form, &$form_state){
  global $hospital_nid;
  $validators = array();
  $row = array();
  $i = 0;
  $sample_array_for_company = array();
  $file_header = 0; // this means that the file has been uploaded without header
  $file_header = $form_state['values']['file_header'];
  $file = file_save_upload('upload', $validators, file_directory_path());
  $csv_uri = $file->filepath;        //file URL
  if (empty($csv_uri)){
    drupal_set_message('No file has been chosen');
    return;
  }
  $handle = fopen($csv_uri, 'r');   //open file -> read only
  while(!feof($handle)){
    $delimiter = $form_state['values']['delimiter'];
    $row = fgetcsv($handle, 1000, $delimiter);
    if (($file_header == 1 && $i !=0) || $file_header == 0){
      file_set_status($file, FILE_STATUS_PERMANENT);
      $node = new stdClass();
      $node->uid = 1;
      $node->created = time();
      $node->type = 'medical_equipment';
      if ($i < 20) {
        $sample_array_for_company[] = $row;
      }
      $client_code = $row[0];
      $client_nid = db_result(db_query('SELECT nid FROM content_type_client WHERE field_client_code_value = "%s"', $client_code));
      $node->field_equipment_client[0]['nid'] = $client_nid;
      $query = 'SELECT item.nid ref_nid, brand.field_type_l1_value brand_id, model.field_type_l2_value model_id, equ_name.field_equipment_name_value equ_name_id FROM content_type_item_data item INNER JOIN content_field_type_l1 brand USING(nid) INNER JOIN content_field_type_l2 model USING(nid) INNER JOIN content_field_equipment_name equ_name USING(nid) WHERE item.field_item_reference_no_value = "%s"';
      $result = db_query($query, $row[1]);
      if ($data = db_fetch_object($result)){
        $item_data_nid = $data->ref_nid;
        $brand_id = $data->brand_id;
        $model_id = $data->model_id;
        $equipment_name_id = $data->equ_name_id;
      }
      $node->field_equipment_coding[0]['nid'] = $item_data_nid;
      $node->field_type_l1[0]['value'] = $brand_id;
      $node->field_type_l2[0]['value'] = $model_id;
      $node->field_equipment_name[0]['value'] = $equipment_name_id;
      $node->field_equ_name_english[0]['value'] = $equipment_name_id;
      //$department = 'DEP';
      $code_id = '1-MED-DEP-' . $i;//_generate_medical_equipment_code_id($hospital_nid, $row[2], $department);
      $node->field_equipment_code[0]['value'] = $code_id;
      if (empty($row[2])){
        $node->title = $node->field_equipment_code[0]['value'];
      }
      else{
        $node->title = $row[2];
      }
      $operation_date = $row[3];
      $date = explode('/', $operation_date);
      $day = $date[0];
      $month = $date[1];
      $year = $date[2];
      $operation_date = $month  ."/". $day ."/". $year;
      $operation_date = strtotime($operation_date);
      $node->field_equipment_operation_date[0]['value'] = $operation_date;
      $node->field_equipment_warranty_months[0]['value'] = $row[4];
      if ($row[4] != 0 && $row[5] != 0){
        $ppm_time_period = $row[4] / $row[5];
      }
      else{
        $ppm_time_period = 0;
      }
      $node->field_equipment_ppm_months[0]['value'] = $ppm_time_period;
    }
    if (!empty($client_nid) && !empty($node->title)){
      _cis_node_save($node);
    }
    $i++;
  }
  if ($form_state['clicked_button']['#id'] =='edit-submit'){
    drupal_set_message($i . t(' Equipment have been imported into the system'));
  }
  else if  ($form_state['clicked_button']['#id'] =='edit-preview'){
    session_start();
    $_SESSION['sample_equipment_upload_for_company'] = $sample_array_for_company;
    drupal_goto('upload/equipment','upload_status=preview');
  }
}

/**
 * upload equipment form element
 */
function upload_equipment_form(){

  $form['guidelines'] = array(
    '#value' => '
                <font color="green">Guidelines to import file into the system</font>
                 <br>1- Convert file to csv file
                 <br>2- Prepare file to be uploaded into the system
                   <br>(a) Insert node like equipment/brand/model ... by id or nid so you have to change the name by id or nid
                   <br>(b) If equipment/brand or model isn\'t in the system, so you have to add the requested node then get node nid to be converted into the CSV file to match the existing data in the system
                   <br>(c) Another values in the file to be set with its values if the field to be set is textfield, no problem .. if the field to be set is select list, you need to convert the statments or the values to numbers to match the select list values in the system
                 <br>3- Take backup from database before the operation of import the file
                 <br>4- After the file has been ready, first review the sample data to be imported in the system, if everythings is fine upload the file again and hit import.!!
                 <br><font color="green">Sample structure</font>
                 <table border=1px style=background-color:#eee><tr>
                 <td>Book</td><td>Page</td><td>Equipment Name</td><td>Agent</td><td>Operation date</td><td>Model</td><td>S.N</td><td>Brand</td><td>Department</td><td>Sub Department</td><td>Price</td><td>Operated by(person 1)</td><td>Equipment</td><td>Furniture</td><td>Operated by (person 2)</td><td>Building</td></tr></table>
                 <br><span style="color:red"><b>Notice : The columns order is very important</b></span>',
  );
  $form['upload_equ'] = array(
    '#title' => 'Upload Equipment',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['upload_equ']['upload'] = array(
    '#type' => 'file',
    '#description' => t('This form allow you to import equipment into system'),
  );
  $form['upload_equ']['file_header'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check this if the file contains the header'),
  );
  $form['upload_equ']['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimiter'),
    '#options' => array(
      ';' => 'Semicolon : ";"',
      ',' => 'Comma : ","',
    ),
    '#required' => 'true',
  );
  $form['upload_equ']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import',
  );
  $form['upload_equ']['preview'] = array(
    '#type' => 'submit',
    '#value' => 'Preview',
  );
  if($_GET['upload_status'] == "preview") {
    $header = array("Book", "Page", "Equipment Name", "Agent", "Operation date", "Model", "S.N", "Brand", "Department", "Sub Department", "Price", "Operated by(person 1)", "Equipment", "Furniture", "Operated by (person 2)", "Building");
    session_start();
    $rows = $_SESSION['sample_equipment_upload'];
  }
  // display table of sapmle data in case of preview status
  if ($_GET['upload_status'] == 'preview') { 
    $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Sample Equipment Upload Table'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
    $html_body .= theme('table', $header, $rows);
    $html_body .= "</div></div>";
    $form['preview'] = array(
      '#value' => $html_body,
    );
  } 
  return $form;
}


    
/**
 * upload equipment form submit button
 */
function upload_equipment_form_submit($form, &$form_state){
  global $hospital_nid;
  $validators = array();
  $sample_array = array();
  $row = array();
  $duplicated_sn = array();
  $i = 0;
  $x = 0;
  $dupplicates = 0;
  $file_header = 0;
  $file_header = $form_state['values']['file_header'];
  $file = file_save_upload('upload', $validators, file_directory_path());
  $csv_uri = $file->filepath;        //file URL
  if (empty($csv_uri)){
    drupal_set_message('No file is chosen');
    return;
  }

  $handle = fopen($csv_uri, 'r'); 
  while(! feof($handle)){
    $delimiter = $form_state['values']['delimiter'];
    $row = fgetcsv($handle, 1000, $delimiter);
    //if (($file_header == 1 && $i !=0) || $file_header == 0){
      file_set_status($file, FILE_STATUS_PERMANENT);
      $node = new stdClass();
      $node->type = 'medical_equipment';
      $node->uid = 1;
      if ($i < 20) {
        $sample_array[] = $row;
      }
      $operation_date = $row[0];
      $date = explode('/', $operation_date);
      $day = $date[0];
      $month = $date[1];
      $year = $date[2];
      $operation_date = $month  ."/". $day ."/". $year;
      $operation_date = strtotime($operation_date);
      $node->field_equipment_operation_date[0]['value'] = $operation_date;
      $code_id = _generate_medical_equipment_code_id($hospital_nid, $row[1], $row[2]);
      $node->field_equipment_code[0]['value'] = $code_id;
      if (empty($row[1])){
        $node->title = $code_id;
      }
      else{
        $node->title = $row[1];
      }
      $node->field_medical_equip_hospital[0]['nid'] = $hospital_nid;
      $node->field_equipment_department[0]['nid'] = $row[2];
      $node->field_equipment_sub_department[0]['nid'] = $row[3];
      $client_nid = db_result(db_query('SELECT nid FROM content_type_client WHERE field_client_code_value = %d', $row[4]));
      $node->field_equipment_client[0]['nid'] = $client_nid;
      $query = 'SELECT item.nid ref_nid, brand.field_type_l1_value brand_id, model.field_type_l2_value model_id, equ_name.field_equipment_name_value equ_name_id FROM content_type_item_data item INNER JOIN content_field_type_l1 brand USING(nid) INNER JOIN content_field_type_l2 model USING(nid) INNER JOIN content_field_equipment_name equ_name USING(nid) WHERE item.field_item_reference_no_value = "%s"';
      $result = db_query($query, $row[5]);
      if ($data = db_fetch_object($result)){
        $item_data_nid = $data->ref_nid;
        $brand_id = $data->brand_id;
        $model_id = $data->model_id;
        $equipment_name_id = $data->equ_name_id;
      }
      $node->field_equipment_coding[0]['nid'] = $item_data_nid;
      $node->field_type_l1[0]['value'] = $brand_id;
      $node->field_type_l2[0]['value'] = $model_id;
      $node->field_equipment_name[0]['value'] = $equipment_name_id;
      $node->field_equ_name_english[0]['value'] = $equipment_name_id;
      if ($form_state['clicked_button']['#id'] =='edit-submit'){
        $query = "SELECT nid, title FROM node WHERE type='medical_equipment' AND title = '{$row[1]}'";
        $result = db_query($query);
        if ($get_row = db_fetch_object($result)){
          if (!empty($get_row)){
            $dupplicates++;
            $equ_link = l('Equipment S.N '. $get_row->title, 'node/'.$get_row->nid);
            drupal_set_message( $equ_link . ' is duplicated');
          }
        }
        else if (!empty($node->title)){
          _cis_node_save($node);
          $nid = $node->nid;
          $x++;
        }      
      }
/*
      $row[11] = iconv("windows-1256" , "utf8" , $row[11]);  // to fix arabic encoding UTF-8
      $row[14] = iconv("windows-1256" , "utf8" , $row[14]);
      if ($i < 20) {
        $sample_array[] = $row;
      }
      $node->type = 'medical_equipment';
      $node->uid = 1;
      $node->field_wordpad_no[0]['value'] = $row[0];
      $node->field_page_no[0]['value'] = $row[1];
      if (is_numeric($row[2])) {
        $node->field_equipment_name[0]['value'] = $row[2];
        $node->field_equ_name_english[0]['value'] = $row[2];
      }
      if (is_numeric($row[3])) {
        $node->field_company_name[0]['nid'] = $row[3];
        $node->field_equipment_base_sup_comp[0]['nid'] = $row[3];
      }
      $operation_date = $row[4];
      $operation_date = strtotime($operation_date);
      $node->field_equipment_operation_date[0]['value'] = $operation_date;
      if (is_numeric($row[5])) {
        $node->field_type_l2[0]['value'] = $row[5];
      }
      $node->field_medical_equip_hospital[0]['nid'] = $hospital_nid;
      $node->field_equipment_building[0]['value'] = $row[15];
      $client_nid = db_result(db_query('SELECT nid FROM content_type_client WHERE field_client_code_value = %d', $row[16]));
      $node->field_equipment_client[0]['nid'] = $client_nid;
      $item_data_nid = db_result(db_query('SELECT nid FROM content_type_item_data WHERE field_item_reference_no_value = "%s"', $row[17]));
      $node->field_equipment_coding[0]['nid'] = $item_data_nid;
      $code_id = _generate_medical_equipment_code_id($hospital_nid, $row[6], $row[8]);
      $node->field_equipment_code[0]['value'] = $code_id;
      if (empty($row[6])){
        $node->title = $code_id;
      }
      else{
        $node->title = $row[6];
      }
      if (is_numeric($row[7])) {
        $node->field_type_l1[0]['value'] = $row[7];
      }
      if (is_numeric($row[8])) {
        $node->field_equipment_department[0]['nid'] = $row[8];
      }
      if (is_numeric($row[9])) {
        $node->field_equipment_sub_department[0]['nid'] = $row[9];
      }
      if (empty($row[10])){
        $node->field_equipment_price[0]['currency'] = '';
      }
      else{
        $node->field_equipment_price[0]['amount'] = $row[10];
      }
      $node->field_operated_by[0]['value'] = $row[11];
      $node->field_operated_by[1]['value'] = $row[14];
      if ($row[13] == 1){
        $node->field_category_l1[0]['value'] = 17197;  // 17197 is the value of furniture
      }
      if ($form_state['clicked_button']['#id'] =='edit-submit'){
        $query = "SELECT nid, title FROM node WHERE type='medical_equipment' AND title = '{$row[6]}'";
        $result = db_query($query);
        if ($get_row = db_fetch_object($result)){
          if (!empty($get_row)){
           $dupplicates++;
           $equ_link = l('Equipment S.N '. $get_row->title, 'node/'.$get_row->nid);
           drupal_set_message( $equ_link . ' is duplicated');
          }
        }
        else if (!empty($node->title) && !empty($row[8])){
          _cis_node_save($node);
          $nid = $node->nid;
          $x++;
        }
      }
*/
      $i++;
    //}
  }
  if ($form_state['clicked_button']['#id'] =='edit-submit'){
    drupal_set_message($i-1 . t(' are the total equipment expected to be imported'));
    drupal_set_message($x . t(' Equipment have been imported into the system'));
    drupal_set_message($dupplicates . t(' Equipment is/are dupplicated'));
  }
  else if  ($form_state['clicked_button']['#id'] =='edit-preview'){ 
    session_start();
    $_SESSION['sample_equipment_upload'] = $sample_array;
    drupal_goto('upload/equipment','upload_status=preview');
  }
}

/**
 * render upload part form
 */
function upload_part(){
  return drupal_get_form('upload_part_form');
}

/**
 * upload part form element
 */
function upload_part_form(){
  if ( $_GET['sheet_ready'] != 1 && ($_GET['upload_status'] == "preview" || $_GET['save_brands_models'] == 1)){
    $cancel = l('Cancel', 'upload/parts');
    $form['brand_model_save_btn'] = array(
      '#type' => 'submit',
      '#value' => 'Save Brands & Models',
    );
    $form['cancel'] = array('#value' => $cancel);
  }
  $form['guidelines'] = array(
    '#value' => '
                <font color="green"><br>Guidelines to import file into the system</font>
                 <br>1- Convert file to csv file
                 <br>2- Prepare file to be uploaded into the system
                   <br>(a) Insert node like equipment/brand/model ... by id or nid so you have to change the name by id or nid
                   <br>(b) If equipment/brand or model isn\'t in the system, so you have to add the requested node then get node nid to be converted into the CSV file to match the existing data in the system
                   <br>(c) Another values in the file to be set with its values if the field to be set is textfield, no problem .. if the field to be set is select list, you need to convert the statments or the values to numbers to match the select list values in the system
                 <br>3- Take backup from database before the operation of import the file
                 <br>4- After the file has been ready, first review the sample data to be imported in the system, if everythings is fine upload the file again and hit import.!!
                 <br><font color="green">Sample structure</font>
                 <table border=1px style=background-color:#eee><tr>
                 <td>Part code</td><td>Part name (e)</td><td>Part name</td><td>Base company supplier</td><td>Brand</td><td>Model</td><td>Cost</td><td>Currency</td><td>Make</td><td>Minimum quantity</td><td>Equipment name</td><td>Unit of measure</td><td>Sell</td></tr></table>
                 <br><span style="color:red"><b>Notice : The columns order is very important</b></span>',
  );
  $form['upload_part'] = array(
    '#title' => 'Upload Parts',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['upload_part']['upload'] = array(
    '#type' => 'file',
    '#description' => t('This form allow you to import parts into system'),
  );
  $form['upload_part']['file_header'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check this if the file contains the header'),
  );
  $form['upload_part']['bypass_validation'] = array(
    '#type' => 'checkbox',
    '#title' => t('By pass Validation'),
  );
  $form['upload_part']['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimiter'),
    '#options' => array(
      ';' => 'Semicolon : ";"',
      ',' => 'Comma : ","',
    ),
    '#required' => 'true',
  );

  $form['upload_part']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import',
  );
  $form['upload_part']['preview'] = array(
    '#type' => 'submit',
    '#value' => 'Preview',
  );
  if($_GET['upload_status'] == "preview") {
    $header = array("Part number", "Part name(e)", "Part arabic Name", "Bas company supplier", "Brand", "Model", "Cost", "Currency", "Make", "Minimum quantity", "Equipment name", "Unit of measure", "Sell");
    session_start();
    $rows = $_SESSION['sample_parts_upload'];
  }
  // display table of sapmle data in case of preview status
  if ($_GET['upload_status'] == 'preview') {
    $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Sample Parts Upload Table'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
    $html_body .= theme('table', $header, $rows);
    $html_body .= "</div></div>";
    $form['preview'] = array(
      '#value' => $html_body,
    );
  }

  return $form;
}

/**
 * upload part form submit button
 */
function upload_part_form_submit($form, &$form_state){
  $models_arr = array();
  $brands_arr = array();
  $parts_sample_array = array();
  $query = 'SELECT term_data.name as brand_name, h1.tid as tid1, h2.tid as tid2 , h3.tid as tid3 FROM term_hierarchy h1 left join term_hierarchy h2 on h1.parent = h2.tid left join term_hierarchy h3 on h2.parent = h3.tid inner join term_data on h1.tid = term_data.tid where vid = 1 and  h2.tid is NULL';
  $result = db_query($query);
  while ($row = db_fetch_object($result)){
    $brands_arr[$row->brand_name] = $row->tid1;
  }
  $query = 'SELECT term_data.name as name, h1.tid as tid1, h2.tid as tid2 , h3.tid as tid3 FROM term_hierarchy h1 left join term_hierarchy h2 on h1.parent = h2.tid left join term_hierarchy h3 on h2.parent = h3.tid inner join term_data on h1.tid = term_data.tid where vid = 1 and  h2.tid > 0';
  $result = db_query($query);
  while ($row = db_fetch_object($result)){
    $models_arr[$row->name] = $row->tid1;
  }
  $validators = array();
  $row = array();
  $brands_not_exist_arr = array();
  $models_not_exist_arr = array();
  $i = 0;
  $row_num = 0;
  $file_header = 0;
  $brand_model_arr_to_save = 0;
  $file_header = $form_state['values']['file_header'];
  $file = file_save_upload('upload', $validators, file_directory_path());
  $csv_uri = $file->filepath;        //file URL
  if (empty($csv_uri)){
    drupal_set_message('No file is chosen');
    return;
  }
  $handle = fopen($csv_uri, 'r');   //open file -> read only
  while(! feof($handle)){
    $delimiter = $form_state['values']['delimiter'];
    $row = fgetcsv($handle, 1000, $delimiter);
    if (($file_header == 1 && $i !=0) || $file_header == 0){
      file_set_status($file, FILE_STATUS_PERMANENT);
      $node = new stdClass();
      if (!empty($row[2]))
        $row[2] = iconv("windows-1256" , "utf8" , $row[2]);  // to fix arabic encoding UTF-8
      if (!empty($row[1]))
        $row[1] = iconv("windows-1256" , "utf8" , $row[1]);  // to fix arabic encoding UTF-8
      if ($i < 20) {
        $parts_sample_array[] = $row;
      }
      $node->type = 'part_catalog';
      $node->uid = 1;
      $node->created = time();
      $node->title = $row[0];
      $node->field_part_data_part_name_e[0]['value'] = $row[1];
      $node->field_part_data_part_name_a[0]['value'] = $row[2];
      $node->field_part_company_sup[0]['nid'] = $row[3];
      $node->field_type_l1[0]['value'] = $brands_arr[$row[4]];
      $node->field_type_l2[0]['value'] = $models_arr[$row[5]];
      $node->field_part_cost[0]['value'] = $row[6];
      $node->field_part_currency_type[0]['value'] = $row[7];
      $node->field_make_text[0]['value'] = $row[8];
      $node->field_part_min_quantity[0]['value'] = $row[9];
      $node->field_equ_name_english[0]['value'] = $row[10];
      $node->field_equipment_name[0]['value'] = $row[10];
      $node->field_part_unit_of_measure[0]['value'] = $row[11];
      $node->field_part_sell[0]['value'] = $row[12];
      if ($form_state['clicked_button']['#id'] =='edit-submit'){
        if (!empty($node->title)){
          _cis_node_save($node);
        }
      }
//print_r($row);
      if (empty($brands_arr[$row[4]]) && !empty($row[4])){
        $brands_not_exist_arr[$row[4]] = $row[4];
        $brand_model_arr_to_save++;
        if ( $form_state['clicked_button']['#id'] == 'edit-preview'){
          drupal_set_message(t('Brand name : ') . $row[4] . t(' is not exist in the system'));
        }
      }
      if (empty($models_arr[$row[5]]) && !empty($row[5])){
        $models_not_exist_arr[$row[5]] = $row[4];
        $brand_model_arr_to_save++;
        if ( $form_state['clicked_button']['#id'] == 'edit-preview'){
          drupal_set_message(t('Model name : ') . $row[5] . t(' is not exist in the system'));
        }
      }
      $bypass_validation = $form_state['values']['bypass_validation'];
      if (!empty($row[0]) && $bypass_validation != 1){
        if ( !is_numeric($row[3]) || !is_numeric($row[7]) || !is_numeric($row[9]) || !is_numeric($row[10]) || !is_numeric($row[12])) {
           $row_num = $i+1;
           form_set_error("", "There are none numeric data in the the following row number $row_num ");
        }
      }
    }
    $i++;
  }//exit();
  if ($form_state['clicked_button']['#id'] =='edit-submit'){
    drupal_set_message($i-1 . t(' Parts have been imported into the system'));
  }
  else if  ($form_state['clicked_button']['#id'] =='edit-preview'){
    if ( $row_num == 0 && $brand_model_arr_to_save == 0){
      drupal_set_message(t('No problem, the sheet is ready to be imported'));
      drupal_goto('upload/parts','upload_status=preview&sheet_ready=1');
    }
    session_start();
    $_SESSION['sample_parts_upload'] = $parts_sample_array;
    drupal_goto('upload/parts','upload_status=preview');
  }
  else if ($form_state['clicked_button']['#id'] == 'edit-brand-model-save-btn' && $_GET['upload_status'] == 'preview'){
    foreach ($brands_not_exist_arr as $key => $value){
      $taxonomy_save_brand_term = array (
        'vid' => 1, // Voacabulary ID
        'name' => $value, // Term Name
        'parent' => '',
      );
      taxonomy_save_term($taxonomy_save_brand_term);
      drupal_set_message(t('Brand name : ') . $value . t(' has been saved into the system'));
    }
    foreach ($models_not_exist_arr as $key => $value){
      $brand_tid = db_result(db_query('SELECT tid FROM term_data INNER JOIN term_hierarchy USING (tid) WHERE parent = 0 and name = "%s"', $value));
      $taxonomy_save_model_term = array (
        'vid' => 1, // Voacabulary ID
        'name' => $key, // Term Name
        'parent' => $brand_tid,
      );
      taxonomy_save_term($taxonomy_save_model_term);
      drupal_set_message(t('Model name : ') . $key . t('has been saved into the system under parent ') . $value);
    }
  }
}

/**
 * get resolved jobs for engineers & group leaders
 */
function get_resolved_jobs($engineer, $date_from, $date_to){
  global $hospital_nid;
  if (!empty($date_from) && !empty($date_to)){
    $and = " AND ((job.field_job_expected_date_of_visit_value >= $date_from AND job.field_job_expected_date_of_visit_value <= $date_to) OR job.field_job_expected_date_of_visit_value is NULL)";
  }
  $query = 'SELECT count(job.nid) resolved_jobs FROM content_type_job job INNER JOIN content_field_job_status status ON (status.nid = job.nid) WHERE job.field_job_assigned_user_uid IN(%d) AND status.field_job_status_value = 6 AND job.field_job_hospital_nid = %d' . $and;
  $result = db_query($query, $engineer, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $resolved_jobs = $row->resolved_jobs;
  }
  return $resolved_jobs;   
}

/**
 * get assigned jobs for engineers & group leaders
 */
function get_assigned_jobs($employee_uid, $date_from, $date_to){
  global $hospital_nid;
  if (!empty($date_from) && !empty($date_to)){
    $and = " AND ((job.field_job_expected_date_of_visit_value >= $date_from AND job.field_job_expected_date_of_visit_value <= $date_to) OR job.field_job_expected_date_of_visit_value is NULL)";
  }
  $query = 'SELECT count(job.nid) assigned_jobs FROM content_type_job job WHERE job.field_job_assigned_user_uid IN (%d) AND job.field_job_hospital_nid = %d' . $and;
  $result = db_query($query, $employee_uid, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $assigned_jobs = $row->assigned_jobs;
  }
  return $assigned_jobs;
}

/**
 * get assigned quotations for engineers & group leaders
 */
function get_assigned_quotations($employee_uid, $date_from, $date_to){
  global $hospital_nid;
  if (!empty($date_from) && !empty($date_to)){
    $and = " AND ((q.field_quotaion_date_value >= $date_from AND q.field_quotaion_date_value <= $date_to) OR q.field_quotaion_date_value is NULL)";
  }
  $query = 'SELECT count(q.nid) assigned_quotations FROM content_type_quotation q WHERE q.field_quotation_related_emp_uid IN (%d) AND q.field_quotation_hospital_nid = %d' . $and;
  $result = db_query($query, $employee_uid, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $assigned_quotations = $row->assigned_quotations;
  }
  return $assigned_quotations;
}

/**
 * get resolved quotations for engineers & group leaders
 */
function get_resolved_quotations($engineer, $date_from, $date_to){
  global $hospital_nid;
  $info_arr = array();
  if (!empty($date_from) && !empty($date_to)){
    $and = " AND ((q.field_quotaion_date_value >= $date_from AND q.field_quotaion_date_value <= $date_to) OR q.field_quotaion_date_value is NULL)";
  }
  $query = 'SELECT count(q.nid) resolved_quotations FROM content_type_quotation q INNER JOIN content_field_job_status status ON (status.nid = q.nid) WHERE q.field_quotation_related_emp_uid IN(%d) AND status.field_job_status_value = 6 AND q.field_quotation_hospital_nid = %d' . $and;
  //$query = 'SELECT count(q.nid) resolved_quotations, working.field_quotation_estimated_workin_value working_hours FROM content_type_quotation q INNER JOIN content_field_job_status status ON (status.nid = q.nid) INNER JOIN content_field_quotation_assigned_employe e ON (e.nid = q.nid) INNER JOIN content_field_quotation_estimated_workin working ON (working.nid = e.nid AND working.delta = e.delta) WHERE e.field_quotation_assigned_employe_uid IN(%d) and q.field_quotation_related_emp_uid IN(%d) AND status.field_job_status_value = 6 AND q.field_quotation_hospital_nid = %d' . $and;
  $result = db_query($query, $engineer, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $info_arr['resolved_quotations'] = $row->resolved_quotations;
    //$info_arr['working_hours']       = $row->working_hours;
  }
  return $info_arr;
}


/**
 * get assigned work orders for engineers & group leaders
 */
function get_assigned_workorders($employee_uid, $date_from, $date_to){
  global $hospital_nid;
  $info_arr = array();
  if (!empty($date_from) && !empty($date_to) && ($date_from != 1 && $date_to != 1000000000000000)){
    $and = " AND wo.field_service_report_vdate_value >= $date_from AND wo.field_service_report_vdate_value <= $date_to";
  }
  $query = 'SELECT sum(wo.field_wrk_order_travelling_hours_value) traveling_hours, sum(wo.field_wrk_order_working_hours_value) working_hours, count(wo.nid) assigned_wo FROM content_type_service_report wo WHERE wo.field_service_report_assigned_em_uid IN (%d) AND wo.field_service_report_hospital_nid = %d' .$and;
  $result = db_query($query, $employee_uid, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $info_arr['assigned_wo']     = $row->assigned_wo;
    $info_arr['working_hours']   = $row->working_hours;
    $info_arr['traveling_hours'] = $row->traveling_hours;
  }
  return $info_arr;
}

/**
 * get assigned pentry for engineers & group leaders
 */
function get_assigned_pentry($employee_uid, $date_from, $date_to){
  global $hospital_nid;
  if (!empty($date_from) && !empty($date_to)){
    $and = " AND pentry.field_pentry_date_value >= $date_from AND pentry.field_pentry_date_value <= $date_to";
  }
  $query = 'SELECT count(pentry.nid) assigned_pentry FROM content_type_pentry pentry WHERE pentry.field_pentry_hospital_person_uid IN (%d) AND pentry.field_pentry_hospital_nid = %d' . $and;
  $result = db_query($query, $employee_uid, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $assigned_pentry = $row->assigned_pentry;
  }
  return $assigned_pentry;
}

/**
 * get resolved pentry for engineers & group leaders
 */
function get_resolved_pentry($employee_uid, $date_from, $date_to){
  global $hospital_nid;
  $pentry_info_arr = array();
  if (!empty($date_from) && !empty($date_to)){
    $and = " AND pentry.field_pentry_date_value >= $date_from AND pentry.field_pentry_date_value <= $date_to";
  }
  // status 0 means ppm visit is done
  $query = 'SELECT sum(pentry.field_pentry_working_hours_value) working_hours, sum(pentry.field_pentry_traveling_hours_value) traveling_hours, count(pentry.nid) resolved_pentry FROM content_type_pentry pentry WHERE pentry.field_pentry_visit_status_value = 0 AND pentry.field_pentry_hospital_person_uid IN (%d) AND pentry.field_pentry_hospital_nid = %d' . $and;
  $result = db_query($query, $employee_uid, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $pentry_info_arr['resolved_pentry'] = $row->resolved_pentry;
    $pentry_info_arr['working_hours']   = $row->working_hours;
    $pentry_info_arr['traveling_hours'] = $row->traveling_hours;
  }
  return $pentry_info_arr;
}

/**
 * get assigned centry for engineers & group leaders
 */
function get_assigned_centry($employee_uid, $date_from, $date_to){
  global $hospital_nid;
  if (!empty($date_from) && !empty($date_to)){
    $and = " AND centry.field_centry_date_value >= $date_from AND centry.field_centry_date_value <= $date_to";
  }
  $query = 'SELECT count(centry.nid) assigned_centry FROM content_type_centry centry WHERE centry.field_hospital_person_uid IN (%d) AND centry.field_centry_hospital_nid = %d'. $and;
  $result = db_query($query, $employee_uid, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $assigned_centry = $row->assigned_centry;
  }
  return $assigned_centry;
}

/**
 * get resolved centry for engineers & group leaders
 */
function get_resolved_centry($employee_uid, $date_from, $date_to){
  global $hospital_nid;
  $centry_info_arr = array();
  if (!empty($date_from) && !empty($date_to)){
    $and = " AND centry.field_centry_date_value >= $date_from AND centry.field_centry_date_value <= $date_to";
  }
  // status 0 means cal visit is done
  $query = 'SELECT sum(centry.field_centry_working_hours_value) working_hours, sum(centry.field_centry_traveling_hours_value) traveling_hours, count(centry.nid) resolved_centry FROM content_type_centry centry WHERE centry.field_centry_visit_status_value = 0 AND centry.field_hospital_person_uid IN (%d) AND centry.field_centry_hospital_nid = %d' . $and;
  $result = db_query($query, $employee_uid, $hospital_nid);
  if ($row = db_fetch_object($result)){
    $centry_info_arr['resolved_centry'] = $row->resolved_centry;
    $centry_info_arr['working_hours']   = $row->working_hours;
    $centry_info_arr['traveling_hours'] = $row->traveling_hours;
  }
  return $centry_info_arr;
}

/**
 * get employee tasks info(CALL-WO-PENTRY_CENTRY) 
 * for engineers & group leaders
 */
function get_employee_tasks_info($employee_uid, $date_from, $date_to){
  $employee_info_arr                        = array();
  $employee_info_arr['assigned_jobs']       = get_assigned_jobs($employee_uid, $date_from, $date_to);
  $employee_info_arr['resolved_jobs']       = get_resolved_jobs($employee_uid, $date_from, $date_to);
  $employee_info_arr['assigned_wo']         = get_assigned_workorders($employee_uid, $date_from, $date_to);
  $employee_info_arr['assigned_quotations'] = get_assigned_quotations($employee_uid, $date_from, $date_to);
  $employee_info_arr['resolved_quotations'] = get_resolved_quotations($employee_uid, $date_from, $date_to);
  $employee_info_arr['assigned_pentry']     = get_assigned_pentry($employee_uid, $date_from, $date_to);
  $employee_info_arr['resolved_pentry']     = get_resolved_pentry($employee_uid, $date_from, $date_to);
  $employee_info_arr['assigned_centry']     = get_assigned_centry($employee_uid, $date_from, $date_to);
  $employee_info_arr['resolved_centry']     = get_resolved_centry($employee_uid, $date_from, $date_to);
  return $employee_info_arr;
}

/**
 * render performance evaluation form
 */
function performance_evaluation(){
  $group     = $_GET['group'];
  $engineer  = $_GET['engineer'];
  $date_from = $_GET['date_from'];
  $date_to   = $_GET['date_to'];
  $where     = '';
  if (!empty($group)){
    $where .= " AND e.field_employee_group_value = $group";
  }
  if (!empty($engineer)){
    $where .= " AND users.uid = $engineer";
  }
  $rows = array();
  $header = array("Engineer Name", "<p style='color:Sienna'>Total Calls</p>", "<p style='color:Sienna'>Resolved Calls</p>", "<p style='color:Sienna'>Opened Calls</p>", "<p style='color:MidnightBlue'>Total Work orders</p>", "<p style='color:MidnightBlue'>Work orders working hours</p>", "<p style='color:MidnightBlue'>Work orders traveling hours</p>", "<p style='color:Maroon'>Total Quotations</p>", "<p style='color:Maroon'>Resolved Quotations</p>", "<p style='color:Maroon'>Opened Quotations</p>", "<p style='color:green'>Total PPMs", "<p style='color:green'>Resolved PPMs", "<p style='color:green'>Opened PPMs", "<p style='color:green'>PPM working hours", "<p style='color:green'>PPM traveling hours", "<p style='color:SlateGray'>Total Calibrations", "<p style='color:SlateGray'>Resolved Calibrations", "<p style='color:SlateGray'>Opened Calibrations", "<p style='color:SlateGray'>Calibration working hours", "<p style='color:SlateGray'>Calibration traveling hours", "View detailed info");
  // 6 means field engineer & 3 refer to group leader
  $query = 'SELECT uid, name FROM users inner join users_roles using (uid) inner join node using (uid) inner join content_type_hospital_employee e ON (e.nid = node.nid) WHERE (users_roles.rid = 6 or users_roles.rid = 3)' . $where;
  $result = db_query($query);
  while ($data = db_fetch_object($result)){
    $row                    = array();
    $employee_path          = l($data->name, 'user/'. $data->uid);
    $row[]                  = $employee_path;
    $employee_info_arr      = get_employee_tasks_info($data->uid, $date_from, $date_to);
    $assigned_call          = $employee_info_arr['assigned_jobs'];
    $row[]                  = $assigned_call;
    $resolved_calls         = $employee_info_arr['resolved_jobs'];
    $row[]                  = $resolved_calls;
    $opened_calls           = $assigned_call - $resolved_calls;
    if ($opened_calls < 0){
      $opened_calls = 0;
    }
    $row[]                  = $opened_calls;
    $assigned_wo            = $employee_info_arr['assigned_wo']['assigned_wo'];
    $working_hours_wo       = $employee_info_arr['assigned_wo']['working_hours'];
    $traveling_hours_wo     = $employee_info_arr['assigned_wo']['traveling_hours'];
    $row[]                  = $assigned_wo;
    $opened_wo              = $assigned_wo - $resolved_wo;
    $row[]                  = round($working_hours_wo, 1);
    $row[]                  = round($traveling_hours_wo, 1);
    $assigned_quotations    = $employee_info_arr['assigned_quotations'];
    $row[]                  = $assigned_quotations;
    $resolved_quotations    = $employee_info_arr['resolved_quotations']['resolved_quotations'];
    $row[]                  = $resolved_quotations;
    $opened_quotations      = $assigned_quotations - $resolved_quotations;
    if ($opened_quotations < 0){
      $opened_quotations = 0;
    }
    $row[]                  = $opened_quotations;
    $assigned_pentry        = $employee_info_arr['assigned_pentry'];
    $row[]                  = $assigned_pentry;
    $resolved_pentry        = $employee_info_arr['resolved_pentry']['resolved_pentry'];
    $pentry_working_hours   = $employee_info_arr['resolved_pentry']['working_hours'];
    $pentry_traveling_hours = $employee_info_arr['resolved_pentry']['traveling_hours'];
    $row[]                  = $resolved_pentry;
    $opened_pentry          = $assigned_pentry - $resolved_pentry;
    if ($opened_pentry < 0){
      $opened_pentry = 0;
    }
    $row[]                  = $opened_pentry;
    $row[]                  = $pentry_working_hours;
    $row[]                  = $pentry_traveling_hours;
    $assigned_centry        = $employee_info_arr['assigned_centry'];
    $row[]                  = $assigned_centry;
    $resolved_centry        = $employee_info_arr['resolved_centry']['resolved_centry'];
    $centry_working_hours   = $employee_info_arr['resolved_centry']['working_hours'];
    $centry_traveling_hours = $employee_info_arr['resolved_centry']['traveling_hours'];
    $row[]                  = $resolved_centry;
    $opened_centry          = $assigned_centry - $resolved_centry;
    if ($opened_centry < 0){
      $opened_centry = 0;
    }
    $row[]                  = $opened_centry;
    $row[]                  = $centry_working_hours;
    $row[]                  = $centry_traveling_hours;
    if (empty($date_from) && empty($date_to)){
      $date_from = 1; $date_to = 1000000000000000;
    }
    $row[]                  = l('View detailed info', 'performance/evaluation/' . $data->uid . '/' . $date_from . '/' . $date_to);
    $rows[]                 = $row;
  }
  if (empty($date_from) && empty($date_to)){
    $date_from = 1; $date_to = 1000000000000000;
  }
  if(empty($group)){
    $group = '-';
  }
  if(empty($engineer)){
    $engineer = '-';
  }
  $path_link = url("evaluation/print/all/$engineer/$group/$date_from/$date_to");
  $img_path = url("sites/default/files/print_icon.png");
  $print_all_link = "<a href=$path_link><img alt='' src=$img_path></img></a>";
  $html_body .= "</div><p style='background-color:Sienna;color:white;width: 35px;float:left;margin-right:20px; text-align:center'>Calls</p>
                       <p style='background-color:MidnightBlue;color:white;width: 75px;float:left;margin-right:20px;text-align:center'>Work orders</p>
                       <p style='background-color:Maroon;color:white;width: 75px;float:left;margin-right:20px; text-align:center'>Quotations</p>
                       <p style='background-color:green;color:white;width: 45px;float:left;margin-right:20px; text-align:center'>PPMs</p>
                       <p style='background-color:SlateGray;color:white;width: 75px;float:left;margin-right:20px; text-align:center'>Calibrations</p> ".$print_all_link ."
                   <div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Performance Evaluation Table'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
  $html_body .= theme('table', $header, $rows);
  $html_body .= "</div></div>";
  return drupal_get_form('performance_evaluation_form') . $html_body;
}

/**
 * print performance evaluation report
 * based on all employees or by group
 */
function performance_evaluation_print_all(){
  if (arg(1) != 'print' && arg(2) != 'all'){
    return;
  }
  $employee_info_arr = array();
  $group     = arg(4);
  $engineer  = arg(3);
  $date_from = arg(5);
  $date_to   = arg(6);
  if ($engineer == '-'){
    $engineer = '';
  }
  if ($group == '-'){
    $group = '';
  }
  $where     = '';
  if (!empty($group)){
    $where .= " AND e.field_employee_group_value = $group";
  }
  if (!empty($engineer)){
    $where .= " AND users.uid = $engineer";
  }
  $rows = array();
  // 6 means field engineer & 3 refer to group leader
  $query = 'SELECT uid, name FROM users inner join users_roles using (uid) inner join node using (uid) inner join content_type_hospital_employee e ON (e.nid = node.nid) WHERE (users_roles.rid = 6 or users_roles.rid = 3)' . $where;
  $result = db_query($query);
  while ($data = db_fetch_object($result)){
    $row                    = array();
    $employee_path          = l($data->name, 'user/'. $data->uid);
    $row[]                  = $data->name;
    $employee_info_arr      = get_employee_tasks_info($data->uid, $date_from, $date_to);
    $assigned_call          = $employee_info_arr['assigned_jobs'];
    $row[]                  = $assigned_call;
    $resolved_calls         = $employee_info_arr['resolved_jobs'];
    $row[]                  = $resolved_calls;
    $opened_calls           = $assigned_call - $resolved_calls;
    if ($opened_calls < 0){
      $opened_calls = 0;
    }
    $row[]                  = $opened_calls;
    $assigned_wo            = $employee_info_arr['assigned_wo']['assigned_wo'];
    $working_hours_wo       = $employee_info_arr['assigned_wo']['working_hours'];
    $traveling_hours_wo     = $employee_info_arr['assigned_wo']['traveling_hours'];
    $row[]                  = $assigned_wo;
    $opened_wo              = $assigned_wo - $resolved_wo;
    $row[]                  = round($working_hours_wo, 1);
    $row[]                  = round($traveling_hours_wo, 1);
    $assigned_quotations    = $employee_info_arr['assigned_quotations'];
    $row[]                  = $assigned_quotations;
    $resolved_quotations    = $employee_info_arr['resolved_quotations']['resolved_quotations'];
    $row[]                  = $resolved_quotations;
    $opened_quotations      = $assigned_quotations - $resolved_quotations;
    if ($opened_quotations < 0){
      $opened_quotations = 0;
    }
    $row[]                  = $opened_quotations;
    $assigned_pentry        = $employee_info_arr['assigned_pentry'];
    $row[]                  = $assigned_pentry;
    $resolved_pentry        = $employee_info_arr['resolved_pentry']['resolved_pentry'];
    $pentry_working_hours   = $employee_info_arr['resolved_pentry']['working_hours'];
    $pentry_traveling_hours = $employee_info_arr['resolved_pentry']['traveling_hours'];
    $row[]                  = $resolved_pentry;
    $opened_pentry          = $assigned_pentry - $resolved_pentry;
    if ($opened_pentry < 0){
      $opened_pentry = 0;
    }
    $row[]                  = $opened_pentry;
    $row[]                  = $pentry_working_hours;
    $row[]                  = $pentry_traveling_hours;
    $assigned_centry        = $employee_info_arr['assigned_centry'];
    $row[]                  = $assigned_centry;
    $resolved_centry        = $employee_info_arr['resolved_centry']['resolved_centry'];
    $centry_working_hours   = $employee_info_arr['resolved_centry']['working_hours'];
    $centry_traveling_hours = $employee_info_arr['resolved_centry']['traveling_hours'];
    $row[]                  = $resolved_centry;
    $opened_centry          = $assigned_centry - $resolved_centry;
    if ($opened_centry < 0){
      $opened_centry = 0;
    }
    $row[]                  = $opened_centry;
    $row[]                  = $centry_working_hours;
    $row[]                  = $centry_traveling_hours;
    if (empty($date_from) && empty($date_to)){
      $date_from = 1; $date_to = 1000000000000000;
    }
    $rows[]                 = $row;
  }
  if ($group == '-' || empty($group)){
    $group = 'Unspecified';
  }
  if ($date_from == 1){
    $date_from = 'Employee started work';
  }
  else{
    $date_from = date('d-m-Y', $date_from);
  }
  if ($date_to == 1000000000000000){
    $date_to = 'Till present';
  }
  else{
    $date_to = date('d-m-Y', $date_to);
  }
  $html_table = "<style>tr{outline: thin solid black;} td{outline: thin solid gray;}</style>
                   <h1 align='center'>Performance Evaluation Report</h1>
                   <p><b>Group : </b>$group</p>
                   <p><b>From : </b>$date_from <b>& To : </b>$date_to</p>
                   <table style='border-style: solid;'>
                     <tr style='background: #ccc;'>
                       <td>Engineer name</td>
                       <td>Total calls</td><td>Resolved calls</td><td>Opened calls</td>
                       <td>Total Work orders</td><td>Resolved work orders</td><td>Opened work orders</td>
                       <td>Total quotations</td><td>Resolved quotations</td><td>Opened quotations</td>
                       <td>Total PPMs</td><td>Resolved PPMs</td><td>Opened PPMs</td><td>PPMs working hours</td><td>PPMs traveling hours</td>
                       <td>Total Calibrations</td><td>Resolved Calibrations</td><td>Opened Calibrations</td><td>Calibrations working hours</td><td>Calibrations traveling hours</td>
                     </tr>";
  foreach ($rows as $key => $value){
    $html_table .= "<tr>
                      <td>$value[0]</td>
                      <td>$value[1]</td><td>$value[2]</td><td>$value[3]</td>
                      <td>$value[4]</td><td>$value[5]</td><td>$value[6]</td>
                      <td>$value[7]</td><td>$value[8]</td><td>$value[9]</td>
                      <td>$value[10]</td><td>$value[11]</td><td>$value[12]</td><td>$value[13]</td><td>$value[14]</td>
                      <td>$value[15]</td><td>$value[16]</td><td>$value[17]</td><td>$value[18]</td><td>$value[19]</td>
                    </tr>";
  }
  $today = date('d/m/Y');
  $html_table .= "</table>" . "<font size=2><br>Report Print Date : " . $today . "</font>";
  print $html_table;exit();
}

/**
 * performance evaluation form elements
 * and engineers' performance in specific period
 */
function performance_evaluation_form(){
  if (!empty($_GET['date_from'])){
    $date_from = date('Y-m-d', $_GET['date_from']);
  }
  if (!empty($_GET['date_to'])){
    $date_to = date('Y-m-d', $_GET['date_to']);
  }
  $checkbox     = $_GET['search_by_group'];
  $group_val    = $_GET['group'];
  $engineer_val = $_GET['engineer'];
  $dateformat   = 'd-m-Y';
  $engineer     = array();
  // 6 means field engineer & 3 refer to group leader
  $query = 'SELECT uid, name FROM users inner join users_roles using (uid) WHERE users_roles.rid = 6 or users_roles.rid = 3'; 
  $result = db_query($query);
  while ($row = db_fetch_object($result)){
    $engineer[$row->uid] = $row->name;
  }
  $group = array();
  $query = 'SELECT Distinct field_employee_group_value FROM content_type_hospital_employee WHERE field_employee_group_value != "Null" ORDER BY field_employee_group_value ASC';
  $result = db_query($query);
  while ($row = db_fetch_object($result)){
    $group[$row->field_employee_group_value] = $row->field_employee_group_value;
  }
  $form['performance']['search_by_group'] = array(
    '#type' => 'checkbox',
    '#title' => t('Search by group'),
    '#default_value' => $checkbox,
  );
  $form['performance']['group_eval'] = array(
    '#type' => 'select',
    '#title' => t('Group'),
    '#options' => array('' => 'None') + $group,
    '#default_value' => $group_val,
  );
  $form['performance']['engineer'] = array(
    '#type' => 'select',
    '#title' => t('Engineer'),
    '#options' => array('' => 'None') + $engineer,
    '#default_value' => $engineer_val,
  );
  $form['performance']['date_from'] = array(
    '#type' => 'date_popup',
    '#date_format' => $dateformat,
    '#default_value' => $date_from,
    '#title' => t('From date'),
  );
  $form['performance']['date_to'] = array(
    '#type' => 'date_popup',
    '#date_format' => $dateformat,
    '#default_value' => $date_to,
    '#title' => t('To date'),
  );
  $form['performance']['apply'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
  );

  return $form;
}

/**
 * apply submit to filter results of the search
 * related to performance_evaluation_form
 */
function performance_evaluation_form_submit($form, &$form_state){
  $checkbox  = $form_state['values']['search_by_group'];
  $group     = $form_state['values']['group_eval'];
  $engineer  = $form_state['values']['engineer'];
  $date_from = strtotime($form_state['values']['date_from']);
  $date_to   = strtotime($form_state['values']['date_to']);
  drupal_goto("performance/evaluation","search_by_group=$checkbox&group=$group&engineer=$engineer&date_from=$date_from&date_to=$date_to");
}

/**
 * Upload equipment name
 */
function upload_equipment_name(){
  return drupal_get_form('upload_equipment_name_form');
}

/**
 * upload equipment name form elements
 */
function upload_equipment_name_form(){
  $form['equ_name'] = array(
    '#title' => 'Upload Equipment Name',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  $form['equ_name']['upload'] = array(
    '#type' => 'file',
    '#description' => t('This form allow you to import equipment into system'),
  );
  $form['equ_name']['file_header'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check this if the file contains the header'),
  );
  $form['equ_name']['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimiter'),
    '#options' => array(
      ';' => 'Semicolon : ";"',
      ',' => 'Comma : ","',
    ),
    '#required' => 'true',
  );
  $form['equ_name']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Import',
  );
  $form['equ_name']['preview'] = array(
    '#type' => 'submit',
    '#value' => 'Preview',
  );
  if($_GET['upload_status'] == "preview") {
    $header = array("Equipment english name", "Equipment arabic name");
    session_start();
    $rows = $_SESSION['sample_equipment_name_upload'];
  }
  // display table of sapmle data in case of preview status
  if ($_GET['upload_status'] == 'preview') {
    $html_body .= "<div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Sample Equipment names from upload'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll' >";
    $html_body .= theme('table', $header, $rows);
    $html_body .= "</div></div>";
    $form['preview'] = array(
      '#value' => $html_body,
    );
  }
  return $form;
}

/**
 * upload equipment name form submit
 */
function upload_equipment_name_form_submit($form, &$form_state){
  $equipment_name_arr = array();
  $validators = array();
  $row = array();
  $i = 0;
  $x = 0;
  $file_header = 0; // this means that the file has been uploaded without header
  $file_header = $form_state['values']['file_header'];
  $file = file_save_upload('upload', $validators, file_directory_path());
  $csv_uri = $file->filepath;        //file URL
  if (empty($csv_uri)){
    drupal_set_message('No file has been chosen');
    return;
  }
  $handle = fopen($csv_uri, 'r');   //open file -> read only
  while(!feof($handle)){
    $delimiter = $form_state['values']['delimiter'];
    $row = fgetcsv($handle, 1000, $delimiter);
    if (($file_header == 1 && $i !=0) || $file_header == 0){
      file_set_status($file, FILE_STATUS_PERMANENT);
      if ($i < 20) {
        $equipment_name_sample_array[] = $row;
      }
      $equipment_name_arr = collect_equipment_name();
      if ($form_state['clicked_button']['#id'] =='edit-submit'){
        if (!empty($row[0]) && !empty($row[1])){
          if (empty($equipment_name_arr['ar'][$row[1]]) && empty($equipment_name_arr['en'][$row[0]])){
            $query = "INSERT INTO equipment_name (arabic_name, english_name) VALUES ('%s', '%s')";
            $result = db_query($query, $row[1], $row[0]);
            $x++;
          }
          else{
            drupal_set_message('Equipment name ' . $row[0] . ' is already in the database.');
          }
        }
      }
    }
    $i++;
  }
  if ($form_state['clicked_button']['#id'] =='edit-submit'){
    drupal_set_message($x . ' Equipment Name have been inserted into database');
  }
  else if  ($form_state['clicked_button']['#id'] =='edit-preview'){
    session_start();
    $_SESSION['sample_equipment_name_upload'] = $equipment_name_sample_array;
    drupal_goto('upload/equipment/name','upload_status=preview');
  }
}

/**
 * collect equipment name
 * that exist in DB
 */
function collect_equipment_name(){
  $equipment_name_arr = array();
  $query = 'SELECT * FROM equipment_name';
  $result = db_query($query);
  while ($row = db_fetch_object($result)){
    $equipment_name_arr['ar'][$row->arabic_name] = $row->id;
    $equipment_name_arr['en'][$row->english_name] = $row->id;
  }
  return $equipment_name_arr;
}
/*
 * render traveling hours frm 
*/
function calculate_traveling_hours(){
  return drupal_get_form('calculate_traveling_hours_frm');
}
/**
 * divide traveling hours frm
 */
function calculate_traveling_hours_frm(){
  $rows = array();
  $header = array('<input type="checkbox" name=travel_check_all" id="travel_checkall">'. t('Check All'), 'Title', 'Type', 'Date');
  $query = 'SELECT nid, title, type, sr.field_service_report_vdate_value srdate, p.field_pentry_actual_date_visit_value pdate, c.field_cal_actual_date_value cdate FROM node LEFT JOIN content_type_service_report sr USING (nid) LEFT JOIN content_type_pentry p USING (nid) LEFT JOIN content_type_centry c USING (nid) WHERE type = "service_report" OR type = "pentry" OR type = "centry"';
  $result = db_query($query);
  while ($data = db_fetch_object($result)){
    $row = array();
    $row[] = '<input type="checkbox" name="travel_check_list['.$nid.']" class="equ_checkbox1" id = '.$id.'value='.$value.'>';
    $row[] = $data->title;
    if ($data->type == 'centry'){
      $type = 'Calibration entry';
    }
    else if ($data->type == 'pentry'){
      $type = 'PPM entry';
    }
    else if ($data->type == 'service_report'){
      $type = 'Work order';
    }
    $row[] = $type;
    $date = 'N/A';
    if (!empty($data->srdate)){
      $date = date('d/m/Y', $data->srdate);
    }
    else if (!empty($data->pdate)){
      $date = date('d/m/Y', $data->pdate);
    }
    else if (!empty($data->cdate)){
      $date = date('d/m/Y', $data->cdate);
    }
    if ($date == '01/01/1970'){
      $date = 'N/A';
    }
    $row[] = $date;
    $rows[] = $row;
  }
  $html_body .= "<div class='portlet box green'>
                   <div class='portlet-title'>
                      <div class='caption'><i class='icon-cogs'></i> ". t('Calculate traveling hours'). "
                                                                                                    </div>
                      <div class='tools'>
                         <a class='collapse' href='javascript:;'></a>
                         <a class='config' data-toggle='modal' href='#portlet-config'></a>
                         <a class='reload' href='javascript:;'></a>
                         <a class='remove' href='javascript:;'></a>
                      </div>
                   </div>
                   <div class='portlet-body lookup-table flip-scroll' >";
  $html_body .= theme('table', $header, $rows);
  $html_body .= "</div></div>";
  $form[]
  return $form;
}
