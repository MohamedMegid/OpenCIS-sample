<?php


/**
 * @author Amr Hesham 1/1/2014 
 * @file cis_system_workflow.inc
 * This file  is included in the cis_system.module file to provide
 * helper functions needed in the whole system and separate the coding from the main
 * module file cis_system.module to anothe file and organize the code in a better way
 * for developers to step in the sysetm .
 */


// define roles constant inside the system 
define('GROUP_LEADER',          3);
define('MANAGER',               4);
define('ADMINSTRATION_MANAGER', 5);
define('FIELD_ENGINEER',        6);
define('SECRETARY',             7);
define('SERVICE_MANAGER',       8);
define('TECHNICIAN',            9);
define('SYS_ADMIN',            10);
define('STORE_MANAGER',            11);


/**
 * Workflow Admin setting for general configuration
 * needed fot the opencis system
 * and allows the user to set different options for system roles
 */

function workflow_admin() {
  $form = array();

  $workflow_options = _return_job_status_values();
  $roles_sql = 'select rid, name from role where rid > 2'; // to ignore anonymous and authenticated users
  $results = db_query($roles_sql);
  while($row = db_fetch_object($results)) {
    $form['wokflow_fieldset_' . $row->rid] = array(
      '#type'  => 'fieldset',
      '#title' => t("$row->name"),
      '#description' => '<em>'. t("Please select different status that this role can view/edit of calls, work orders,quotation and PO") . '</em>',
    );

    // list status that this role can edit
    $form['wokflow_fieldset_' . $row->rid] ['workflow_status_role_edit' . $row->rid] = array(
      '#type' => 'select',
      '#title' => t('Workflow status that can edit'),
      '#options' => $workflow_options,
      '#default_value' => variable_get('workflow_status_role_edit' . $row->rid, 3),
      '#multiple' => true, 
    );
    // list status that this role can view
    $form ['wokflow_fieldset_' . $row->rid] ['workflow_status_role_view' . $row->rid] = array(
      '#type' => 'select',
      '#title' => t('Workflow status that can viewed by') ,
      '#options' => $workflow_options,
      '#default_value' => variable_get('workflow_status_role_view' . $row->rid, 3),
      '#multiple' => true, 
    );

    // CALL Notification
    $form['wokflow_fieldset_' . $row->rid] ['call_notification_' . $row->rid] = array(
      '#type' => 'select',
      '#title' => t('Call Notification'),
      '#options' => $workflow_options,
      '#default_value' => variable_get('call_notification_' . $row->rid, 3),
      '#multiple' => true,
      '#description' => t('please choose which status you want to send notification on for Call')
    );
    
    // Work Order Notification
    $form['wokflow_fieldset_' . $row->rid] ['work_order_notification_' . $row->rid] = array(
      '#type' => 'select',
      '#title' => t('Work Order Notification'),
      '#options' => $workflow_options,
      '#default_value' => variable_get('work_order_notification_' . $row->rid, 3),
      '#multiple' => true,
      '#description' => t('please choose which status you want to send notification on for Work Order')
    );

    // Quotation Notification
    $form['wokflow_fieldset_' . $row->rid] ['quotation_notification_' . $row->rid] = array(
      '#type' => 'select',
      '#title' => t('Quotation Notification'),
      '#options' => $workflow_options,
      '#default_value' => variable_get('quotation_notification_' . $row->rid, 3),
      '#multiple' => true,
      '#description' => t('please choose which status you want to send notification on for quotation')
    );
    // Purchase Order Notification
    $form['wokflow_fieldset_' . $row->rid] ['importcode_notification_' . $row->rid] = array(
      '#type' => 'select',
      '#title' => t('Purchase Order Notification'),
      '#options' => $workflow_options,
      '#default_value' => variable_get('importcode_notification_' . $row->rid, 3),
      '#multiple' => true,
      '#description' => t('please choose which status you want to send notification on for Purchase Order')
    );
  }
  return system_settings_form($form);
}

/**
 * Notifications Admin setting for general configuration
 * needed fot the opencis system
 * and allows the user to set different options for system roles
 */
function notification_admin() {
  
    return system_settings_form($form);
}


/**
 * Employees groups assignment Admin settings 
 * needed fot the opencis system
 */

function employees_groups_admin() {
  $form = array();
  $form['#submit'][] = 'employees_groups_admin_submit';
  $form['#validate'][] = 'employees_groups_admin_validate';
  // $workflow_options = _return_job_status_values();
  $key = 'field_employee_group';
  $content_field = content_fields($key);
  $allowed_values = content_allowed_values($content_field);

  foreach ($allowed_values as $key => $group_name) {
    $form['emp_groups_fieldset_' . $key] = array(
      '#type'  => 'fieldset',
      '#title' => t("$group_name"),
      '#description' => '<em>'. t("Please select group leader and members of every group") . '</em>',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    // list status that this role can edit
    $form['emp_groups_fieldset_' . $key] ['selected_group_leader_' . $key] = array(
      '#type' => 'select',
      '#title' => t('Group Leader'),
      '#options' => _get_group_leaders(),
      '#default_value' => variable_get('selected_group_leader_' . $key, 3),
    );
    // list status that this role can edit
    $form['emp_groups_fieldset_' . $key] ['selected_group_members_' . $key] = array(
      '#type' => 'select',
      '#title' => t('Group Members'),
      '#options' => _get_fe_tech_users(),
      '#default_value' => variable_get('selected_group_members_' . $key, 3),
      '#multiple' => true, 
    );
  }
  $form['reassign_calls'] = array(
    '#type' => 'checkbox',
    '#title' => t('ReAssign calls to the new group members and free old one'),
    '#default_value' => variable_get('reassign_calls', 1),
    '#options' => array(0 => t(''), 1=> t('ReAssign calls to the new group members')),
  );
  return system_settings_form($form);
}



/**
 * Admin setting for general configuration
 * needed fot the opencis system
 * and allows the user to set different options for the system such as
 * type of installation, notifications, warranty and so on .
 */
function cmms_admin() {
  $form = array();

  $form['country'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#default_value' => variable_get('country', 0),
    '#description' => t("Please specify the country that the corporation belongs to"),
    '#options' => array('Egypt' => t('Egypt'), 'KSA'=> t('KSA'), 'Germany' => t('Germany') , 'Emirates' => t('Emirates')),
    '#required' => TRUE,
  );

  $form['higher_corporation_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Higher Corporation Name'),
    '#default_value' => variable_get('higher_corporation_name', ''),
    '#description' => t("Please specify the higher corporation name that the corporation belongs to"),
    '#required' => TRUE,
  );

  $form['higher_corporation_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Higher Corporation Code'),
    '#default_value' => variable_get('higher_corporation_code', ''),
    '#description' => t("Please specify the generated code for the higher corporation that the corporation belongs to"),
    '#required' => TRUE,
  );

  $form['Sub_Enterprise_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Sub-Enterprise Name'),
    '#default_value' => variable_get('Sub_Enterprise_name', ''),
    '#description' => t("Please specify the corporation name"),
    '#required' => TRUE,
  );

  $form['Sub_Enterprise_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Sub-Enterprise Code'),
    '#default_value' => variable_get('Sub_Enterprise_code', ''),
    '#description' => t("Please specify the generated code for the corporation"),
    '#required' => TRUE,
  );

  $form['cmms_type'] = array(
    '#type' => 'select',
    '#title' => t('Installation type of cmms'),
    '#default_value' => variable_get('cmms_type', 3),
    '#description' => t("Please select the installation type of cmms"),
    '#options' => array(0 => t('General Hospital'), 1=> t('Hospital Heirarichal Org'), 2 => t('Medical Company') , 3 => t('General Maintenance Company')),
    '#required' => TRUE,
  );

 $form['cmms_system_path'] = array(
    '#type' => 'textfield',
    '#title' => t('CMMS URL'),
    '#default_value' => variable_get('cmms_system_path', "http://196.218.124.206/life_care_cmms/"),
    '#description' => t("Please enter the url / base path for the system"),
    '#required' => TRUE,
  );

 $form['cmms_internal_system_path'] = array(
    '#type' => 'textfield',
    '#title' => t('CMMS internal URL'),
    '#default_value' => variable_get('cmms_internal_system_path', "http://196.218.124.206/life_care_cmms/"),
    '#description' => t("Please enter the url / base path for the system"),
    '#required' => TRUE,
  );
 

  $form['cmms_perm_type'] = array(
    '#type' => 'select',
    '#title' => t('User Permission based on'),
    '#default_value' => variable_get('cmms_perm_type', 3),
    '#description' => t("Please select permission type that cmms will work upon it."),
    '#options' => array(0 => t('Department'), 1=> t('Group'), 2 => t('Equipment Name') , 3 => t('None')),
    '#required' => TRUE,
  );

  $form['warranty_end_date_based_on'] = array(
    '#type' => 'select',
    '#title' => t('Warranty end date based on'),
    '#default_value' => variable_get('warranty_end_date_based_on', 3),
    '#description' => t("please select warranty end date based on"),
    '#options' => array(0 => t('operation'), 1=> t('Supply and operation')),
    '#required' => TRUE,
  );
  $form['mcontract_end_reminder_notification'] = array(
    '#type' => 'select',
    '#title' => t('Maintenance contracts Notitification period'),
    '#default_value' => variable_get('mcontract_end_reminder_notification', 77),
    '#description' => t("please select the notification period which the system will send you mail/sms on it"),
    '#options' => array(
            1 => t('1 month'), 
            2 => t('2 months'),
            3 => t('3 months'),
            4 => t('4 months'),
            5 => t('5 months'),
            6 => t('6 months'),
            7 => t('7 months'),
            8 => t('8 months'),
            9 => t('9 months'),
            10 => t('10 months')
    ),
    '#required' => TRUE,
  );
  $form['mcontracts_renew_period'] = array(
    '#type' => 'select',
    '#title' => t('Maintenance contracts period before renewing'),
    '#default_value' => variable_get('mcontracts_renew_period', 3),
    '#description' => t("please select the priod in which the system will show up a link for renewing contract"),
    '#options' => array(
            1 => t('1 month'), 
            2 => t('2 months'),
            3 => t('3 months'),
            4 => t('4 months'),
            5 => t('5 months'),
            6 => t('6 months'),
            7 => t('7 months'),
            8 => t('8 months'),
            9 => t('9 months'),
            10 => t('10 months')
    ),
    '#required' => TRUE,
  );
  $form['quotation_cost_based_on'] = array(
    '#type' => 'select',
    '#title' => t('Quotation cost based on'),
    '#default_value' => variable_get('quotation_cost_based_on', 3),
    '#description' => t("please select quotation cost based on"),
    '#options' => array(0 => t('last quotation price'), 1=> t('factor')),
  );
  $form['delete_ppm_cal_inv_based_on'] = array(
    '#type' => 'select',
    '#title' => t('Delete PPM, Calibration and invoices at Maintenance contracts for any change of the data based on'),
    '#default_value' => variable_get('delete_ppm_cal_inv_based_on', 3),
    '#description' => t("please select deletion based on"),
    '#options' => array(0 => t('Automatic deletion'), 1=> t('Manual deletion')),
  );

  $form['automatic_ppm_call_based_warranty'] = array(
    '#type' => 'checkbox',
    '#title' => t('PPM, Calibration to be generated based on warranty'),
    '#description' => t("basic feature and required for any feature depended on PPM & Calibration"),
    '#default_value' => variable_get('automatic_ppm_call_based_warranty', 0),
    '#options' => array(0 => t(''), 1=> t('Enable automatic ppm and cal genration based warranty')),
  );

  $form['automatic_ppm_call_based_mcontract'] = array(
    '#type' => 'checkbox',
    '#title' => t('PPM, Calibration to be generated based on Maintenance contracts'),
    '#description' => t("basic feature and required for any feature depended on PPM & Calibration"),
    '#default_value' => variable_get('automatic_ppm_call_based_mcontract', 0),
    '#options' => array(0 => t(''), 1=> t('Enable automatic ppm and cal genration based Maintenance contract')),
  );


  $form['contract_end_date'] = array(
    '#type' => 'checkbox',
    '#title' => t('PPM/Calibration end date is the maintenance contracts end date'),
    '#description' => t("The system will create the PPM / Calibration plan and assign the end date for the plan to be the same end date of the contract (note: otherwise the PPM/Calibration plan end date will be the end date of current year and in this case the user has to choose the automatic PPM/Calibration generation option)."),
    '#default_value' => variable_get('contract_end_date', 0),
    '#options' => array(0 => t(''), 1=> t('Contract end date')),
  );


  $form['quotation_contract_functionality'] = array(
    '#type' => 'checkbox',
    '#title' => t('List price/Quotation for maintenance contracts.'),
    '#description' => t("The system will show an option in the maintenance contracts for the user to choose between list price/quotation and normal contract <b>(@company)</b>."),
    '#default_value' => variable_get('quotation_contract_functionality', 0),
    '#options' => array(0 => t(''), 1=> t('Enable quotation contract functionality')),
  );

  $form['filled_assigned_employee_automatic'] = array(
    '#type' => 'checkbox',
    '#title' => t('Group leader automatic assignment in PPM/Calibration during warranty'),
    '#description' => t("The system will automatical detect the group leader of the client using this medical equipment and will select it automatically for new PPM/Calibration to be generated <b>(@company)</b>"),
    '#default_value' => variable_get('filled_assigned_employee_automatic', 0),
    '#options' => array(0 => t(''), 1=> t('Assigned employee will be filled automatic based on Group leader at Medical equipment')),
  );
  
  $form['filled_assigned_employee_automatic_mc'] = array(
    '#type' => 'checkbox',
    '#title' => t('Group leader automatic assignment in PPM/Calibration in maitnenance contract'),
    '#description' => t("The system will automatical detect the group leader of the client using this medical equipment and will select it automatically for new PPM/Calibration to be generated <b>(@company)</b>"),
    '#default_value' => variable_get('filled_assigned_employee_automatic_mc', 0),
    '#options' => array(0 => t(''), 1=> t('Assigned employee will be filled automatic based on Group leader at Maintenace contract')),
  );

  $form['target_contracts_feature'] = array(
    '#type' => 'checkbox',
    '#title' => t('Target contracts functionality'),
    '#description' => t("The system allow the company to specify the targeted contracts to be made for specific equipment and models and assign them to sales engineer users <b>(@company)</b>"),
    '#default_value' => variable_get('target_contracts_feature', 0),
    '#options' => array(0 => t(''), 1=> t('Target contracts feature')),
  );

  $form['maintenance_costs_feature'] = array(
    '#type' => 'checkbox',
    '#title' => t('Maintenance Income functionality'),
    '#description' => t("the system  provide a report specifying the amount of money/income achieved from the maintenance department in the company according to warranty, maintenance contracts,purchase order <b>(@company)</b>"),
    '#default_value' => variable_get('maintenance_costs_feature', 0),
    '#options' => array(0 => t(''), 1=> t('Maintenance Costs feature')),
  );

  $form['automatic_user_assigned'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatic assign call to user/employee responsible for the client of that call.'),
    '#description' => t("the user/employee have field of the client who is responsible for, and the system automtically assign calls to that user based on clients on his profile <b>(@company)</b>"),
    '#default_value' => variable_get('automatic_user_assigned', 0),
    '#options' => array(0 => t(''), 1=> t('Enable automatic assignemt for call')),
  );

  $form['automatic_ppm_call_based_contract_approval'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cron - generation of ppm/call and invoices for the contract in every year the contract is renewed'),
    '#description' => t("the system has a background service that is running every day to check if there are contracts to be renewed and to generate PPM/Calibration or invoices automatically  for them <b>(@company)</b>"),
    '#default_value' => variable_get('automatic_ppm_call_based_contract_approval', 0),
    '#options' => array(0 => t(''), 1=> t('create ppm and calibration based on maintenance contract automatically')),
  );

  $form['automatic_coding_based_initials'] = array(
    '#type' => 'checkbox',
    '#title' => t('Equipment code based on client and model initials'),
    '#description' => t("The system generate code for equipment as Clientcode-Model-Serial <b>(@company)</b>"),
    '#default_value' => variable_get('automatic_coding_based_initials', 0),
    '#options' => array(0 => t(''), 1=> t('Enable automatic coding')),
  );

  $form['mcontract_code_generation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Maintenance contract code/id based on client code'),
    '#description' => t("Maintenance contracts code to be generated automatical based on client code i.e Clientcode-Year-Serial<b>(@company)</b>"),
    '#default_value' => variable_get('mcontract_code_generation', 0),
    '#options' => array(0 => t(''), 1=> t('generate code based on client code (@medical_company)')),
  );

  $form['enable_user_location'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable user Location'),
    '#description' => t("the system detect the user location from the IP address, and find the city and country of the current location of the user, user should allow access to his location "),
    '#default_value' => variable_get('enable_user_location', 0),
    '#options' => array(0 => t(''), 1=> t('enable user location')),
  );
  
  $form['hide_unit_price_and_quotation_type_wo'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide unit price and quotation type in Work Order'),
    '#description' => t("Hide unit price and quotation type from spare part information in work order<b>(@company)</b>"),
    '#default_value' => variable_get('hide_unit_price_and_quotation_type_wo', 0),
    '#options' => array(0 => t(''), 1=> t('Hide unit price and quotation type in Work Order')),
  );

  $form['requestion_part_catalog_functionality'] = array(
    '#type' => 'checkbox',
    '#title' => t('Requesition report of part catalog according to Quotations & PO(s)'),
    '#description' => t("List part number, description, current stock, related opened PO(s) & Quotations that don't have PO(s) <b>(@company)</b>."),
    '#default_value' => variable_get('requestion_part_catalog_functionality', 0),
    '#options' => array(0 => t(''), 1=> t('Part catalog requestion for Quotations & PO(s)')),
  );

  $form['add_group_leader_in_call'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add group leader in Call'),
    '#default_value' => variable_get('add_group_leader_in_call', 0),
    '#description' => t("Add group leader in call in order to recieve notifications and reports, and it will be automatic assigned based on client <b>(@company)</b>"),
    '#options' => array(0 => t(''), 1=> t('Add group leader in Call request')),
  );


  $form['show_maintenance_info_based_on_first_action_in_call'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show maintenance info tab based on first action tab in call'),
    '#default_value' => variable_get('show_maintenance_info_based_on_first_action_in_call', 0),
    '#description' => t(" Maintenance info tab will be hidden by default and will be shown in first action status of 'Need A quotation' <b>(@company)</b>"),
   '#options' => array(0 => t(''), 1=> t('Show maintenance info tab based on first action tab in call')),
  );

  $form['create_invoice_based_on_po'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create invoice based on PO to affect the store (part catalog quantity)'),
    '#default_value' => variable_get('create_invoice_based_on_po', 0),
    '#description' => t(" Provide invoice functionality to be added to the system"),
    '#options' => array(0 => t(''), 1=> t('Create invoice based on PO to affect the store (part catalog quantity')),
  );

  $form['same_total_in_quotation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Validation to set the same total of print info and spare parts in a quotation'),
    '#default_value' => variable_get('same_total_in_quotation', 0),
    '#description' => t("The same total of print info items count and thier prices with the total of spare parts in a quotation form"),
    '#options' => array(0 => t(''), 1=> t('Validation to set the same total of print info and spare parts in a quotation')),
  );   
  $form['caller_telephone'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add caller telephone in call'),
    '#default_value' => variable_get('caller_telephone', 0),
    '#description' => t("The ability to use caller telephone field in call form"),
    '#options' => array(0 => t(''), 1=> t('Add caller telephone in call')),
  );
  
  $form['group_leader_review_option'] = array(
    '#type' => 'checkbox',
    '#title' => t('Group Leader have the ability to review tasks'),
    '#description' => t("the system allows the group leader to change the status of tasks to be reviewed by group leader or not"),
    '#default_value' => variable_get('group_leader_review_option', 0),
    '#options' => array(0 => t(''), 1=> t('Group Leader Review tasks option')),
  );


  $form['spare_part_store'] = array(
    '#type' => 'checkbox',
    '#title' => t('show spare part as text autocomplete'),
    '#description' => t("show spare part as text autocomplete instead of part catalog reference from database, this used for hospital and organizations that does not required store "),
    '#default_value' => variable_get('spare_part_store', 0),
    '#options' => array(0 => t(''), 1=> t('Show spare part autocomplete')),
  );

  $form['insert_org_code_in_title_code'] = array(
    '#type' => 'checkbox',
    '#title' => t('Insert organization code into code of CALL, QUOTATION, PO & INVOICE'),
    '#description' => t("Insert organization code into code of CALL, QUOTATION, PO & INVOICE i.e : node_definition-org_code-current_year-S.N"),
    '#default_value' => variable_get('insert_org_code_in_title_code', 0),
    '#options' => array(0 => t(''), 1=> t('Insert organization code into code of CALL, QUOTATION, PO & INVOICE')),
  );

  $form['show_message_notification_icon'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show message notification icon like facebook'),
    '#description' => t("Show mesaages notification icon like facebook to allow the user to cancel task to be as a readed task"),
    '#default_value' => variable_get('show_message_notification_icon', 0),
    '#options' => array(0 => t(''), 1=> t('Show message notification icon like facebook')),
  );

  $form['send_sms_client_contact_person'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send SMS to client\'s contact person'),
    '#description' => t("Send SMS to client's contact person in case of creating Call"),
    '#default_value' => variable_get('send_sms_client_contact_person', 0),
    '#options' => array(0 => t(''), 1=> t('Send SMS to client\'s contact person')),
  );

  $form['maintenance_cost_functionality_in_quotation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show maintenance cost functionality in quotation'),
    '#description' => t("Calculate maintenance cost in quotation based on estimated working hours of employees"),
    '#default_value' => variable_get('maintenance_cost_functionality_in_quotation', 0),
    '#options' => array(0 => t(''), 1=> t('Show maintenance cost functionality in quotation')),
  );

  $form['performance_evaluation_report'] = array(
    '#type' => 'checkbox',
    '#title' => t('The system generates performance evaluation report about engineer'),
    '#description' => t("The system generates performance report to evaluate engineers"),
    '#default_value' => variable_get('performance_evaluation_report', 0),
    '#options' => array(0 => t(''), 1=> t('The system generates performance evaluation report about engineer')),
  );

  $form['renew_mc_add_years_to_enddate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Renew Maintenance contract by adding years to the end date of the contract'),
    '#description' => t("Renew Maintenance contract by adding years to the end date of the contract(link)"),
    '#default_value' => variable_get('renew_mc_add_years_to_enddate', 0),
    '#options' => array(0 => t(''), 1=> t('Renew Maintenance contract by adding years to the end date of the contract')),
  );
  $form['replace_equipment_form'] = array(
    '#type' => 'checkbox',
    '#description' => t("Give the user the facility to make Five Year plan for replacing old operated equipment"),
    '#title' => t('Show/hide Equipment Replace form'),
    '#default_value' => variable_get('replace_equipment_form', 0),
    '#options' => array(0 => t(''), 1=> t('Show/hide Replacement and Renewal form')),
  );

  $form['show_second_sn_in_call'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show second serial number in call form'),
    '#description' => t("'Show second serial number in call form (Specified to Surgitech)"),
    '#default_value' => variable_get('show_second_sn_in_call', 0),
    '#options' => array(0 => t(''), 1=> t('Show second serial number in call form')),
  );

  return system_settings_form($form);
}

/**
 * returns workflow status
 */
function _return_workflow_status() {
  $workflow_sql = 'select id, name from {workflow_status}';
  $results = db_query($workflow_sql);
  $workflow_options = array();
  while($row = db_fetch_object($results)) {
    $workflow_options[$row->id] = $row->name;
  }
  return $workflow_options;

}


/**
 * Gets equipment information by coding nid of data item input
 * @param $code_nid
 * @retrun
 *   equipment name
 *   brand
 *   model
 *   make
 */
function _get_equipment_info_by_coding() {
 // where are we going to play with other p
  $code_nid = arg(1);
  if (!empty($code_nid) && is_numeric($code_nid)) {
    $item_data_node = node_load($code_nid);
    drupal_json(array(           
      'equipment_name' => $item_data_node->field_equipment_name[0]['value'],
      'brand'          => $item_data_node->field_type_l1[0]['value'],
      'model'          => $item_data_node->field_type_l2[0]['value'],
      'make'           => $item_data_node->field_make_text[0]['value'],
      'supplier'       => $item_data_node->field_item_company_sup[0]['nid']));
  }

}


function employees_groups_admin_validate ($form, &$form_state) {

}

/**
 * get list of users assigned to group one by one
 */
function employees_groups_admin_submit($form, &$form_state) {
  // user_list array looks like
  /* 
      2,35,67,7
      4,9,20,34
      87,6,66,22
   */
  $users_list            = array();
  $validation_users      = array(); // array to avoid dupplication
  $users_comma_separated ='';
  $key                   = 'field_employee_group';
  $content_field         = content_fields($key);
  $allowed_values        = content_allowed_values($content_field);
  $group_leaders         = array();
  foreach ($allowed_values as $key => $group_name) {
    $group_leader = $form_state['values']['selected_group_leader_' . $key] ;
    if (array_key_exists($group_leader, $validation_users)) {
      form_set_error("" , $message = 'Dupplicate GROUP LEADER', $reset = FALSE);
      return;
      $group_leaders[$key] = $group_leader;
    }
    else {
      $validation_users[$group_leader] = $group_leader;
    }
    $users_comma_separated .= $group_leader . ',';
    foreach ($form_state['values']['selected_group_members_' . $key] as $uid => $user_id) {
      $users_comma_separated .= $user_id . ',';
      if (array_key_exists($user_id, $validation_users)) {
      form_set_error("", $message = 'Dupplicate user in the same group', $reset = FALSE);
      return;
      }
      else {
        $validation_users[$user_id] = $user_id;
      }
    }
    $users_list[$key] = substr($users_comma_separated, 0, -1);
    $users_comma_separated = '';
  }

  _update_users_group($users_list);
  _empty_call_assigned_emp();
  _empty_ppm_assigned_emp();
  _empty_cal_assigned_emp(); 
  _send_mails_to_group_leaders($users_list);
  cache_clear_all();
 
}

/**
 * this function empty the assigned user from 
 * call if it is not belonging to the group
 * of that call
 */
function _empty_call_assigned_emp() {
  // return users who are not in the client group
  $query = "select content_type_job.nid, content_field_job_status.field_job_status_value ,content_type_job.field_job_assigned_user_uid, content_type_client.field_client_group_value as client_group , emp.field_employee_group_value as user_group from content_type_hospital_employee emp inner join node on (emp.nid = node.nid) inner join users on (users.uid = node.uid) inner join content_type_job  on (content_type_job.field_job_assigned_user_uid = users.uid) inner join content_field_job_client  on (content_field_job_client.nid = content_type_job.nid) inner join content_type_client  on (content_type_client.nid = content_field_job_client.field_job_client_nid) inner join content_field_job_status on (content_field_job_status.nid = content_type_job.nid) where field_employee_group_value != field_client_group_value and content_field_job_status.field_job_status_value in (4,5)";

  $result = db_query($query);
  while ($row = db_fetch_array($result)) {
    db_query("update content_type_job set field_job_assigned_user_uid = NULL where nid = %d", $row['nid']);  
    $call_link = l('View call', 'node/'.$row['nid']);
    drupal_set_message("Call has been updated, click the following link  " . $call_link );
  }

}


/**
 * this function empty the assigned user from 
 * PPM if it is not belonging to the group
 * of that call
 */
function _empty_ppm_assigned_emp() {
  // return users who are not in the client group
  $query = "select content_type_pentry.nid,content_type_pentry.field_pentry_hospital_person_uid,content_type_pentry.field_pentry_visit_status_value , content_type_client.field_client_group_value as client_group , emp.field_employee_group_value as user_group from content_type_hospital_employee emp inner join node on (emp.nid = node.nid) inner join users on (users.uid = node.uid) inner join content_type_pentry on (content_type_pentry.field_pentry_hospital_person_uid = users.uid) inner join content_field_job_client on (content_field_job_client.nid = content_type_pentry.nid) inner join content_type_client on (content_type_client.nid = content_field_job_client.field_job_client_nid)  where field_employee_group_value != field_client_group_value  and content_type_pentry.field_pentry_visit_status_value  in (1,2)
";

  $result = db_query($query);
  while ($row = db_fetch_array($result)) {
    db_query("update content_type_pentry set field_pentry_hospital_person_uid  = NULL where nid = %d", $row['nid']);
    $call_link = l('View ppm', 'node/'.$row['nid']);
    drupal_set_message("PPM has been updated, click the following link  " . $call_link );
  }

}

/**
 * this function empty the assigned user from 
 * calibration if it is not belonging to the group
 * of that call
 */
function _empty_cal_assigned_emp() {
  // return users who are not in the client group
  $query = "select content_type_centry.nid,content_type_centry.field_hospital_person_uid,content_type_centry.field_centry_visit_status_value  , content_type_client.field_client_group_value as client_group , emp.field_employee_group_value as user_group from content_type_hospital_employee emp inner join node on (emp.nid = node.nid) inner join users on (users.uid = node.uid) inner join content_type_centry on (content_type_centry.field_hospital_person_uid = users.uid) inner join content_field_job_client on (content_field_job_client.nid = content_type_centry.nid) inner join content_type_client on (content_type_client.nid = content_field_job_client.field_job_client_nid) where field_employee_group_value != field_client_group_value  and content_type_centry.field_centry_visit_status_value in (1,2)";

  $result = db_query($query);
  while ($row = db_fetch_array($result)) {
    db_query("update content_type_centry set field_hospital_person_uid  = NULL where nid = %d", $row['nid']);
    $call_link = l('View cal', 'node/'.$row['nid']);
    drupal_set_message("CAL has been updated, click the following link  " . $call_link );
  }

}

/**
 * this function send mails to group leader 
 */
function _send_mails_to_group_leaders($users_list) {
  $users_comma_separated = implode(',', $users_list);
  $select_query = "select node.uid, emp.field_employee_mobile_number_value as mobile, users.mail as mail from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r on r.uid = node.uid  inner join users on users.uid = node.uid where  node.uid in ($users_comma_separated)";
  //print $select_query;
  $results = db_query($select_query);
  $users = array();
  while($row = db_fetch_array($results)){
    $uid = $row['uid'];
    $users[$uid] = array ('uid'    => $row['uid'],
                   'mobile' => $row['mobile'],
                   'mail'   => $row['mail'] ) ; // get group leader of the current group
                          //print_r($result); exit();
  }

  $mail_recipients = _get_recipients_mails_concatenated($users, ",");
  $mail_type = UPDATE_EMPLOYEES_GROUP_MOVEMENTS;
  $subject = "EMPLOYEES GROUP MOVEMENTS";
  _notifications_send_routing_email($node, $mail_type, $status , $subject, $mail_recipients, $user->name);

}
/**
 * helper function to update the users groups 
 * after new assignment
 */
function _update_users_group($users_list) {

  foreach ($users_list as $key => $value) {
    $update_query = "update content_type_hospital_employee e inner join node on node.nid=e.nid  set   e.field_employee_group_value = $key where node.uid in ($value) and node.type = 'hospital_employee'";
    db_query($update_query);
    drupal_set_message("Employee groups assignement has been update for Group #$key");
  }

}

/**
 * function to return users who will
 * recieve notifications
 */
function _return_users_to_receive_notification_based_workflow_config($node,$call_nid,$service_report_nid) {
   
  $node_type = '';
  switch($node->type) {
    case 'quotation':
      $node_type = 'quotation_notification_';
      break;
    case 'job':
      $node_type = 'call_notification_';
      break;
    case 'service_report':
      $node_type = 'work_order_notification_';
      break;
    case 'importcode':
      $node_type = 'importcode_notification_';
      break;
  }
  $status = $node->field_job_status[0]['value'];
  $result = array();
  $total = array();
  $get = db_query("select field_job_client_nid  from content_type_job inner join content_field_job_client using (nid) where nid = %d", $call_nid);
  if ($get_nid = db_fetch_object($get)){
    $client = $get_nid->field_job_client_nid;
  }
  // iterate over all roles 
  for ($i = 3; $i <= 10; $i++){  // loop to get roles from 3 = group leader to 10
    $role_status = variable_get("$node_type".$i, 20);
    $group_leader_config = variable_get('add_group_leader_in_call', 0);
    if ($i == GROUP_LEADER && $role_status[$status] == $status && $group_leader_config != 1 ){
      $query = db_query("select field_client_group_value from content_type_client where nid = %d", $client);
      if($row = db_fetch_object($query)) {
        $group_of_client = $row->field_client_group_value;  // get group of client
      }
     // print "select node.uid, emp.field_employee_mobile_number_value as mobile, users.mail as mail from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r on r.uid = node.uid  inner join users on users.uid = node.uid where r.rid = 3 AND emp.field_employee_group_value = %d" .$group_of_client; exit();
      $query2 = db_query("select node.uid, emp.field_employee_mobile_number_value as mobile, users.mail as mail from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r on r.uid = node.uid  inner join users on users.uid = node.uid where r.rid = 3 AND emp.field_employee_group_value = %d", $group_of_client);
      if($res = db_fetch_array($query2)){
        $group_leader = $res['uid'];
        $result[$group_leader] = array ('uid'    => $res['uid'],
                           'mobile' => $res['mobile'],
                           'mail'   => $res['mail'] ) ; // get group leader of the current group
                           //print_r($result); exit();
      }

    }

    else if($i == GROUP_LEADER && $role_status[$status] == $status && $group_leader_config == 1){
      if ($node_type == 'call_notification_' && arg(1) == 'add'){
        $gleader_uid = $node->field_job_group_leader[0]['uid'];
        $query = db_query('SELECT emp.field_employee_mobile_number_value mobile, users.mail mail FROM users INNER JOIN node ON node.uid = users.uid INNER JOIN content_type_hospital_employee emp ON emp.nid = node.nid where users.uid = %d', $gleader_uid);
        if ($get = db_fetch_array($query)){
          $result[$gleader_uid] = array ('uid'    => $gleader_uid,
                             'mobile' => $get['mobile'],
                             'mail'   => $get['mail'] ) ;
        }
      }
      else if ($node_type == 'importcode_notification_' && arg(1) == 'add'){
        $query = db_query('select node.uid as uid, emp.field_employee_mobile_number_value as mobile, users.mail as mail from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r on r.uid = node.uid  inner join users on users.uid = node.uid inner join content_type_job job ON (job.field_job_group_leader_uid = node.uid) where r.rid = 3 AND job.nid = %d', $call_nid);
        if ($get = db_fetch_array($query)){
          $gleader_uid = $get['uid'];
          $result[$gleader_uid] = array ('uid'    => $gleader_uid,
                             'mobile' => $get['mobile'],
                             'mail'   => $get['mail'] ) ;
        }
      }
      if ($node_type == 'call_notification_' && arg(2) == 'edit'){
        global $original_loaded_node;
        $pre_assigned_emp = $original_loaded_node->field_job_assigned_user[0]['uid'];
        $current_assigned_emp = $node->field_job_assigned_user[0]['uid'];
        if (empty($pre_assigned_emp)){
          $query = db_query('SELECT emp.field_employee_mobile_number_value mobile, users.mail mail FROM users INNER JOIN node ON node.uid = users.uid INNER JOIN content_type_hospital_employee emp ON emp.nid = node.nid where users.uid = %d', $current_assigned_emp);
          if ($get2 = db_fetch_array($query)){
            $result[$current_assigned_emp] = array ('uid'    => $current_assigned_emp,
                             'mobile' => $get2['mobile'],
                             'mail'   => $get2['mail'] ) ;
          }
        }
        $new_gl = $node->field_job_group_leader[0]['uid'];
        $group_leader_arr_info = return_group_leader_if_changed($node, $original_loaded_node);
        if (!empty($group_leader_arr_info)){
          $result[$new_gl] = $group_leader_arr_info;
          global $original_loaded_node;
          $gl_uid = $original_loaded_node->field_job_group_leader[0]['uid'];
          $query = db_query('SELECT emp.field_employee_mobile_number_value mobile, users.mail mail FROM users INNER JOIN node ON node.uid = users.uid INNER JOIN content_type_hospital_employee emp ON emp.nid = node.nid where users.uid = %d', $gl_uid);
          if ($get2 = db_fetch_array($query)){
            $result[$gl_uid] = array ('uid'    => $gl_uid,
                             'mobile' => $get2['mobile'],
                             'mail'   => $get2['mail'] ) ;
          }
        }
        else{
          $gleader_uid = $node->field_job_group_leader[0]['uid'];
          $query5 = db_query('SELECT emp.field_employee_mobile_number_value mobile, users.mail mail FROM users INNER JOIN node ON node.uid = users.uid INNER JOIN content_type_hospital_employee emp ON emp.nid = node.nid where users.uid = %d', $gleader_uid);
          if ($get = db_fetch_array($query5)){
            $result[$gleader_uid] = array ('uid'    => $gleader_uid,
                               'mobile' => $get['mobile'],
                               'mail'   => $get['mail'] ) ;
          }
        }
      }
    }
    elseif(($i == FIELD_ENGINEER || $i == TECHNICIAN  )&&  $role_status[$status] == $status ){
      if (is_numeric($service_report_nid)) {
        $query3 = db_query("select s.field_service_report_assigned_em_uid as uid,users.mail as mail, emp.field_employee_mobile_number_value as mobile from content_type_service_report s inner join users on users.uid = s.field_service_report_assigned_em_uid inner join node on node.uid=users.uid inner join content_type_hospital_employee  emp on emp.nid = node.nid  where s.nid = %d", $service_report_nid);
        if($row1 = db_fetch_array($query3)){
          $assigned_employee = $row1['uid'];
        }   
        if($assigned_employee == $group_leader){
          $assigned_employee = "";
        }
        else{
          $key = $row['uid'];
          $result[$key] = array ('uid'    => $row1['uid'],
                           'mobile' => $row1['mobile'],
                           'mail'   => $row1['mail'] ) ; // get the field engineer that the quotation is assigned to
        }
      }
      else {
        $assigned_user = $node->field_job_assigned_user[0]['uid'];
        $query = db_query("select users.uid,name,users.mail ,e.field_employee_mobile_number_value  as mobile from users inner join node using(uid) inner join content_type_hospital_employee e on (e.nid = node.nid) where uid = %d", $assigned_user);
        if ($row = db_fetch_array($query)) {
          $key = $row['uid'];
          $result[$key] = array ('uid' => $assigned_user ,
                         'mobile' => $row['mobile'],
                         'mail'   => $row['mail']) ;
        }
      }
    }
    elseif(($i != GROUP_LEADER || $i != FIELD_ENGINEER || $i != TECHNICIAN)   && $role_status[$status] == $status ){
      $query3 = db_query("select DISTINCT node.uid, emp.field_employee_mobile_number_value as mobile, users.mail as mail from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r on r.uid = node.uid  inner join users on users.uid = node.uid where rid = %d", $i );
      while($row = db_fetch_array($query3)){
        $key = $row['uid'];
        $result[$key] = array ('uid'   => $row['uid'],
                           'mobile' => $row['mobile'],
                           'mail'   => $row['mail'] ) ;
        
      }
    }
  }
  //print_r($result);
  return $result;  // result of group leader, field engineer and other employees
}

/**
 * return the new group leader information if the 
 * old group leader for that call has been changed
 */
function return_group_leader_if_changed($node, $original_loaded_node){
  global $original_loaded_node;
  if ($original_loaded_node->field_job_group_leader[0]['uid'] != $node->field_job_group_leader[0]['uid']){
    $gl_uid = $node->field_job_group_leader[0]['uid'];
    $query = db_query('SELECT emp.field_employee_mobile_number_value mobile, users.mail mail FROM users INNER JOIN node ON node.uid = users.uid INNER JOIN content_type_hospital_employee emp ON emp.nid = node.nid where users.uid = %d', $gl_uid);
    if ($result = db_fetch_array($query)){
      $leader = array ('uid'    => $gl_uid,
                         'mobile' => $result['mobile'],
                         'mail'   => $result['mail'] ) ;
    }
  }
  return $leader;
}

/**
* return the new assigned employee information if the
* old assigned employee for that call has been changed
*/
function return_assigned_employee_if_changed($node, $original_loaded_node){
  global $original_loaded_node;
  if ($original_loaded_node->field_job_assigned_user[0]['uid'] != $node->field_job_assigned_user[0]['uid']){
    $assigned_user = $node->field_job_assigned_user[0]['uid'];
    $query = db_query("select users.uid,name,users.mail ,e.field_employee_mobile_number_value  as mobile from users inner join node using(uid) inner join content_type_hospital_employee e on (e.nid = node.nid) where uid = %d", $assigned_user);
    if ($row = db_fetch_array($query)) {
      $assigned_employee = array ('uid' => $assigned_user ,
                             'mobile' => $row['mobile'],
                             'mail'   => $row['mail']) ;
    }
    return $assigned_employee;
  }
}
        


/**TODO NEED TO BE REMOVED, BUT MAKE SURE IT IS NOT BEING USED
 * function to determine employees whome to send
 *   notification in quotation case
 */
function _return_employees_to_send_notifications_quotation_case($node) {
  $status = $node->field_job_status[0]['value'];
  $result = array();
  $total = array();
  $call_nid = $node->field_quotation_job[0]['nid'];
  $get = db_query("select field_job_client_nid from content_type_job where nid =" . $call_nid);
  if ($get_nid = db_fetch_object($get)){
    $client = $get_nid->field_job_client_nid;
  }
  for ($i = 3; $i <= 10; $i++){  // loop to get roles from 3 = group leader to 10
    if ($i == 3){
      $query = db_query("select field_client_group_value from content_type_client where nid =".$client);
      if($row = db_fetch_object($query)) {
        $group_of_client = $row->field_client_group_value;  // get group of client
      }
      $query2 = db_query("select node.uid from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r where r.rid = 3 AND r.uid = node.uid emp.field_employee_group_value =". $group_of_client);
      if($res = db_fetch_object($query2)){
        $group_leader = $res->uid;
        $result[] = $group_leader;  // get group leader of the current group
      }

    }
    if($i == 6){
      $assigned_employee = $node->field_service_report_assigned_em[0]['uid'];
      if($assigned_employee == $group_leader){
        $assigned_employee = "";
      }
      else{
        $result[] = $assigned_employee;  // get the field engineer that the quotation is assigned to
      }
    }
     if($i != 3 || $i != 6){
      $role = variable_get('quotation_notification_'.$i, 3);
      $total[$i] = $role;
      if ($status == $total[$i][$status]){
        $query3 = db_query("select DISTINCT uid from users_roles where rid = ". $i );
        if($rows = db_fetch_array($query3)){
          $employees = $rows['uid'];
        }
        $result[] = $employees;  // get the employees in the current quotation
      }
    }
  print_r($result); print "tet"; exit();
  return $result;  // result of group leader, field engineer and other employees
  }
}
 
/**TODO NEED TO BE REMOVED, BUT MAKE SURE IT IS NOT BEING USED
 * function to determine employees whome to send
 *   notification in work order case
 */
function _return_employees_to_send_notifications_work_order_case($node) {
  $status = $node->field_job_status[0]['value'];
  $result = array();
  $total = array();
  $call_nid = $node->field_importcode_job[0]['nid'];
  $get = db_query("select field_job_client_nid from content_type_job where nid =" . $call_nid);
  if ($get_nid = db_fetch_object($get)){
    $client = $get_nid->field_job_client_nid;
  }
  for ($i = 3; $i <= 10; $i++){  // loop to get roles from 3 = group leader to 10
    if ($i == 3){
      $query = db_query("select field_client_group_value from content_type_client where nid =".$client);
      if($row = db_fetch_object($query)) {
        $group_of_client = $row->field_client_group_value;  // get group of client
      }
      $query2 = db_query("select node.uid from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r where r.rid = 3 AND r.uid = node.uid emp.field_employee_group_value =". $group_of_client);
      if($res = db_fetch_object($query2)){
        $group_leader = $res->uid;
        $result[] = $group_leader;  // get group leader of the current group
      }

    }
    if($i == 6){
      $assigned_employee = $node->field_service_report_assigned_em[0]['uid'];
      if($assigned_employee == $group_leader){
        $assigned_employee = "";
      }
      else{
        $result[] = $assigned_employee;  // get the field engineer that the work order is assigned to
      }
    }

    if($i != 3 || $i != 6){
      $role = variable_get('work_order_notification_'.$i, 3);
      $total[$i] = $role;
      if ($status == $total[$i][$status]){
        $query3 = db_query("select DISTINCT uid from users_roles where rid = ". $i );
        if($rows = db_fetch_array($query3)){
          $employees = $rows['uid'];
        }
        $result[] = $employees;  // get the employees in the current work order
      }
    }
  return $result;  // result of group leader, field engineer and other employees
  }
}

/**TODO NEED TO BE REMOVED, BUT MAKE SURE IT IS NOT BEING USED
 *  function to determine employees whome to send
 *   notification in purchase order case
 */
function _return_employees_to_send_notifications_purchase_order_case($node) {
  $status = $node->field_job_status[0]['value'];
  $result = array();
  $total  = array();
  $call_nid = $node->field_service_report_job[0]['nid'];
  $get = db_query("select field_job_client_nid from content_type_job where nid =" . $call_nid);
  if ($get_nid = db_fetch_object($get)){
    $client = $get_nid->field_job_client_nid;
  }
  for ($i = 3; $i <= 10; $i++){  // loop to get roles from 3 = group leader to 10
    if ($i == 3){
      $query = db_query("select field_client_group_value from content_type_client where nid =".$client);
      if($row = db_fetch_object($query)) {
        $group_of_client = $row->field_client_group_value;  // get group of client
      }
      $query2 = db_query("select node.uid from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r where r.rid = 3 AND r.uid = node.uid emp.field_employee_group_value =". $group_of_client);
      if($res = db_fetch_object($query2)){
        $group_leader = $res->uid;
        $result[] = $group_leader;  // get group leader of the current group
      }

    }
    if($i == 6){
      $assigned_employee = $node->field_service_report_assigned_em[0]['uid'];
      if($assigned_employee == $group_leader){
        $assigned_employee = "";
      }
      else{
        $result[] = $assigned_employee;  // get the field engineer that the purchase order is assigned to
      }
    }
     if($i != 3 || $i != 6){
      $role = variable_get('importcode_notification_'.$i, 3);
      $total[$i] = $role;
      if ($status == $total[$i][$status]){
        $query3 = db_query("select DISTINCT uid from users_roles where rid = ". $i );
        if($rows = db_fetch_array($query3)){
          $employees = $rows['uid'];
        }
        $result[] = $employees;  // get the employees in the current purchase order
      }
    }
  return $result;  // result of group leader, field engineer and other employees
  }
}

/** TODO need to be removed
 * function to determine employees whome to send 
 *  notification in call case
 */
function _return_employees_to_send_notifications_call_case($node) {
  $status = $node->field_job_status[0]['value'];
  $result = array();
  $total  = array();
  for ($i = 3; $i <= 10; $i++){  // loop to get roles from 3 = group leader to 10
    if ($i == 3){
      $client = $node->field_job_client[0]['nid'];
      $query  = db_query("select field_client_group_value from content_type_client where nid =".$client);
      if($row = db_fetch_object($query)) {
        $group_of_client = $row->field_client_group_value;  // get group of client
      }
      $query2 = db_query("select node.uid from node inner join content_type_hospital_employee emp using (nid) inner join users_roles r where r.rid = 3 AND r.uid = node.uid emp.field_employee_group_value =". $group_of_client);
      if($res = db_fetch_object($query2)){
        $group_leader = $res->uid;
        $result[] = $group_leader;  // get group leader of the current group
      }

    }
    if($i == 6){
      $assigned_employee = $node->field_job_assigned_user[0]['uid'];
      if($assigned_employee == $group_leader){
        $assigned_employee = "";
      }
      else{
        $result[] = $assigned_employee;  // get the field engineer that the call is assigned to
      }
    }

    if($i != 3 || $i != 6){
      $role = variable_get('call_notification_'.$i, 3);
      $total[$i] = $role;
      if ($status == $total[$i][$status]){
        $query3 = db_query("select DISTINCT uid from users_roles where rid = ". $i );
        if($rows = db_fetch_array($query3)){
          $employees = $rows['uid'];
        }
        $result[] = $employees;  // get the employees in the current call
      }
    }
  return $result;  // result of group leader, field engineer and other employees
  }
}


/**
  Gets user's departments
 * @param $uid
 *   user id
 * 
 * @return $nids 
 *   nids of departments that the user is assigned to work in .
 */
function _get_user_departments($uid) {
  global $user;
  //print_r($user->roles); exit();
  // check for the permission type to use this option or not0.
  if ($uid ==1 || !empty($user->roles[5])  || !empty($user->roles[8])  || !empty($user->roles[4]) ) {
    $ret= 'all';
    return $ret;
  }
  if (variable_get('cmms_perm_type', array()) == 0) {
    $query = "select  d.field_employee_department_nid as department_nid from content_field_employee_department d inner join node on node.nid=d.nid where node.uid = %d and node.type = 'hospital_employee'";
    $result = db_query($query, $uid);
    $department_nids_array = array();

    while ($row = db_fetch_array($result)) {
      $department_nids_array[] = $row['department_nid'];
    }

    if (count($department_nids_array) > 0) {

     //return 'all';
      return implode('+', $department_nids_array);
    }
    else {


    }
  }
  else return 'all';
}



/**
 * Gets user's assigned equipment name
 * @param $uid
 *   user id
 * 
 * @return $nids 
 *   nids of departments that the user is assigned to work in .
 */
function _get_user_assigned_equipment($uid) {
  global $user;
  //print_r($user->roles); exit();
  // check for the permission type to use this option or not0.
  if ($uid ==1 || !empty($user->roles[5])  || !empty($user->roles[8])  || !empty($user->roles[4]) ) {
    $ret= 'all';
    return $ret;
  }
  if (variable_get('cmms_perm_type', array()) == 2) {
    $query = "select  e.field_employee_equipment_value as equipment_value from content_field_employee_equipment e inner join node on node.nid=e.nid where node.uid = %d and node.type = 'hospital_employee'";
    $result = db_query($query, $uid);
    $equipment_ids_array = array();

    while ($row = db_fetch_array($result)) {
      $equipment_ids_array[] = $row['equipment_value'];
    }

    if (count($equipment_ids_array) > 0) {

      //return '430+427'; 
      return implode('+', $equipment_ids_array);
    }
    else {


    }
  }
  else return 'all';
}



function _get_loggedin_user_group($uid) {
  if (variable_get('cmms_perm_type', array()) == 1) {
    $query = "select  e.field_employee_group_value as group_id from content_type_hospital_employee e inner join node on node.nid=e.nid where node.uid = %d and node.type = 'hospital_employee'";
    $result = db_query($query, $uid);

    if ($row = db_fetch_array($result)) {
      $group_id = $row['group_id'] ;
      return $group_id;
    }

  }
}

/**
 * Gets user's assigned group
 * @param $uid
 *   user id
 * 
 * @return $group_id 
 *   group id of the user
 */
function _get_user_assigned_group($uid) {
  global $user;
  //print_r($user->roles); exit();
  // check for user role.
  //$uid = $user->uid;
  if ($uid ==1 || !empty($user->roles[MANAGER])  || !empty($user->roles[SERVICE_MANAGER])  || !empty($user->roles[ADMINSTRATION_MANAGER])
    || !empty($user->roles[SECRETARY]) || !empty($user->roles[SYS_ADMIN]) ) {
    $ret= 'all';
    return $ret;
  }
  if (variable_get('cmms_perm_type', array()) == 1) {
    $query = "select  e.field_employee_group_value as group_id from content_type_hospital_employee e inner join node on node.nid=e.nid where node.uid = %d and node.type = 'hospital_employee'";
    $result = db_query($query, $uid);

    if ($row = db_fetch_array($result)) {
      $group_id = $row['group_id'] ;
      return $group_id;
    }

  }
  else return 'all';
}


/**
 * Gets users of specific group
 * @param 
 *   group id
 * 
 * @return $users array
 */
function _get_users_group_list($groupid) {

  $query = "select users.name,users.uid  from users inner join node using(uid) inner join content_type_hospital_employee e  on node.nid=e.nid   inner join users_roles on users_roles.uid = users.uid where node.type = 'hospital_employee' and rid in (6,9) and e.field_employee_group_value =%d";
  $result = db_query($query, $groupid);
  $users_list = array();
  while ($row = db_fetch_array($result)) {
    $users_list[] = $row['uid'];
  }
  return $users_list;
}


/**
 * return group leader users
 */
function _get_group_leaders() {
  
  $query = "select users.name,users.uid  from users  inner join users_roles on users_roles.uid = users.uid where rid=%d";
  $result = db_query($query, GROUP_LEADER);
  $users_list = array();
  while ($row = db_fetch_object($result)) {
    $users_list[$row->uid] = $row->name;
  }
  return $users_list;


}

/**
 * return all Field engineer and Tech
 */
function _get_fe_tech_users() {
  
  $query = "select users.name,users.uid  from users  inner join users_roles on users_roles.uid = users.uid where rid in (6,9)";
  $result = db_query($query, GROUP_LEADER);
  $users_list = array();
  while ($row = db_fetch_object($result)) {
    $users_list[$row->uid] = $row->name;
  }
  return $users_list;


}




/**
 * Gets user's assigned group using this ajax function
 * @param $uid
 *   user id
 * 
 * @return $group_id 
 *   group id of the user
 */
function _get_users_group() {

  $groupid = arg(1);
  $users_group = array();
  if(arg(2) == 'view') {
    $users_group_to_be_send= "<option value=\"All\">- Any -</option>";
  }
  else {
    $users_group_to_be_send= "<option value=\"\">- None -</option>";
  }
 
  if (arg(1) > 0) { 
    $query = "select users.name,users.uid  from users inner join node using(uid) inner join content_type_hospital_employee e  on node.nid=e.nid   inner join users_roles on users_roles.uid = users.uid where node.type = 'hospital_employee' and rid in (6,9) and e.field_employee_group_value =%d";
  }
  else {
    $query = "select users.name,users.uid  from users inner join node using(uid) inner join content_type_hospital_employee e  on node.nid=e.nid inner join users_roles on users_roles.uid = users.uid where node.type = 'hospital_employee' and rid in (6,9)  ";
  }
  //echo $query;
  $result = db_query($query, $groupid);
  while ($row = db_fetch_array($result)) {
    $users_group_to_be_send .= "<option value =". $row['uid'] . ">" . $row['name']  . "</option>";
  }
  echo $users_group_to_be_send;
}



/**
 * Get's job status
 * @param $job_nid
 *
 * @return 
 *  green for resolved
 *  yelow for under working
 *  red for no result
 */
function _get_job_status( $job_nid) {
  $query = 'select field_job_status_value status from content_field_job_status where nid=%d';
  $result = db_query($query, $job_nid);
  if ($row = db_fetch_array($result)) {
    $status = $row['status'];
 

    if ($status == 3 || $status == 2 || $status == 6 || $status == 8)
      return 'green';
    else {
      return 'yellow';
    }
  }
  else {
    return 'red'; // means there is no reocrds
  }

}



/**
 * check if there service report / work order has been
 * created for this job or not 
 *
 * @param $job_nid
 *   nid of the job
 */
function _check_service_report_exist_for_job($job_nid) {
  $query = 'select nid from content_type_service_report where field_service_report_job_nid=%d order by nid DESC';
  $result = db_query($query, $job_nid);
  if ($row = db_fetch_array($result)) 
    return $row['nid'];
  
  else return false;

}


/**
 * check if there quotation has been
 * created for this job or not 
 *
 * @param $job_nid
 *   nid of the job
 */
function _check_quotation_exist_for_job($job_nid) {
  $query = 'select nid from content_type_quotation where field_quotation_job_nid=%d order by nid DESC';
  $result = db_query($query, $job_nid);
  if ($row = db_fetch_array($result)) 
    return $row['nid'];
  
  else return false;
}


/**
 * check if there import code has been
 * created for this job or not 
 *
 * @param $job_nid
 *   nid of the job
 */
function _check_importcode_exist_for_job($job_nid) {
  $query = 'select nid from content_type_importcode where field_importcode_job_nid=%d order by nid DESC';
  $result = db_query($query, $job_nid);
  if ($row = db_fetch_array($result)) 
    return $row['nid'];
  
  else return false;
}



/**
 * Returns the htlm needed to represent
 * workflow status in colored circles above 
 * every call , work order , quotation and importcode
 * to let the user knows exactly where is the workflow
 * statnding on which stage
 */
function _return_workflow_status_chart_html($job_nid ) {
  
  $witdth            = 0;
  $job_status        = _get_job_status($job_nid);
  $width             = $width + 25;
  // check if there is service report
  $job_url           = url('node/' . $job_nid);
  $servicereport_nid = _check_service_report_exist_for_job($job_nid);
  if (is_numeric($servicereport_nid)) {// todo 
    $servicereport_status = _get_job_status($servicereport_nid);
    $servicereport_url    = url('node/' . $servicereport_nid);
    $width                = $width + 25;
  }
  else $servicereport_status = 'red'; 
  $importcode_nid = _check_importcode_exist_for_job($job_nid);
  if (is_numeric($importcode_nid)) { // todo
    $importcode_url    = url('node/' . $importcode_nid); 
    $importcode_status = _get_job_status($importcode_nid);
    $width             = $width + 25;
  }
  else  { 
    $importcode_status = 'red'; 
  }
  $quotation_nid = _check_quotation_exist_for_job($job_nid);
  if (is_numeric($quotation_nid)) { // todo
    $quotation_url    = url('node/' . $quotation_nid); 
    $quotation_status = 'green';
    $width            = $width + 25;
  }
  else  {
    $quotation_status = 'red';
  }
  if ($job_status =='green')
    $width = 100;
    
 
  echo '
                     <div class="portlet-body form">
                        <form action="#" class="form-horizontal">
                           <div class="form-wizard">
                              <div class="navbar steps">
                                 <div class="navbar-inner">
                                    <ul class="row-fluid">
                                       <li class="span3">
                                          <a href="'.$job_url.'"  class="step ">
                                          <span class="number '.$job_status.'">1</span>
                                          <span class="desc"><i class="icon-ok"></i>'. t('Call (Request)').'</span>   
                                          </a>
                                       </li>
                                       <li class="span3">
                                          <a href="'.$servicereport_url.'" class="step">
                                          <span class="number '.$servicereport_status.'">2</span>
                                          <span class="desc"><i class="icon-ok"></i>'.t('Work Order').'</</span>   
                                          </a>
                                       </li>
                                       <li class="span3">
                                          <a href="'. $quotation_url.'"  class="step">
                                          <span class="number '.$quotation_status.'">3</span>
                                          <span class="desc"><i class="icon-ok"></i>'.t('Quotation').'</span>   
                                          </a>
                                       </li>
                                       <li class="span3">
                                          <a href="'.$importcode_url.'" class="step">
                                          <span class="number '.$importcode_status.'">4</span>
                                          <span class="desc"><i class="icon-ok"></i>'.t('Purchase Order').'</span>   
                                          </a> 
                                       </li>
                                    </ul>
                                 </div>
                              </div>
                              <div id="bar" class="progress progress-success progress-striped">
                                 <div class="bar" style="width: '.$width.'%; border:white;"></div>
                              </div>

                          </div>
                        </form>
                     </div>
';
}



/**
 * function handles the issues related to transfer
 * employee content type and workflow needed actions
 */
function _handle_transfer_employee($node) {
  // check the user taking action
  // if the user is administration manager or other agreed role to
  // add transfer employee
  global $user;
  if (!empty($user->roles[5])) {
    // send an email to the service line manager in order to approve the movement
  }
  elseif (!empty($user->roles[8])) {
    // clear assigned employee field in open jobs , and ppm , calibration
    // get nids for open jobs
    
    // get nids for open ppm visits
    // get nids for open calibration visits
    // change group for every employee according to the new selections
    // send en email notification for every new assigned group leader
    // to have a view for the new opened jobs which is displayed according to groups assigned
  }

}


/**
 * if there are any changes of warranty months, extended,
 * ppm time period, cal or operation date
 * delete the old ppm, cal and generate new
 */
function delete_old_ppm_cal_and_generate_new(&$node){
  if (arg(2) == 'edit'){
   // delete all records for that equipment and generate new
    $ppm_nid                       = _check_if_contract_has_ppm_done($node);
    $cal_nid                       = _check_if_contract_has_cal_done($node);
    $operation_date_node           = $node->field_equipment_operation_date[0]['value'];
    $warranty_months_node          = $node->field_equipment_warranty_months[0]['value'];
    $extended_warranty_months_node = $node->field_equipment_ext_warranty_mon[0]['value'];
    $ppm_time_period_node          = $node->field_equipment_ppm_months[0]['value'];
    $cal_time_period_node          = $node->field_equipment_cal_months[0]['value'];
    global $original_eq_node;
    $operation_date                = $original_eq_node->field_equipment_operation_date[0]['value'];
    $warranty_months               = $original_eq_node->field_equipment_warranty_months[0]['value'];
    $extended_warranty_months      = $original_eq_node->field_equipment_ext_warranty_mon[0]['value'];
    $ppm_time_period               = $original_eq_node->field_equipment_ppm_months[0]['value'];
    $cal_time_period               = $original_eq_node->field_equipment_cal_months[0]['value'];
    if (($operation_date_node != $operation_date) || ($warranty_months_node != $warranty_months) || ($extended_warranty_months_node != $extended_warranty_months) || ($ppm_time_period_node != $ppm_time_period) || ($cal_time_period_node != $cal_time_period) && ($ppm_nid < 0 || $cal_nid < 0)){
      handle_equipment_delete($node);
      _handle_warranty_ppm_cal($node);
      insert_into_equipmentmaintenance_cost($node);
    }
  }
}

/** 
 * save ppm and calibration enteries for equipment
 * after defininig the warranty information 
 */
function _handle_warranty_ppm_cal($node) {
  
  if (variable_get('automatic_ppm_call_based_warranty', 0) == 1) {
    $warranty_end_date = $node->field_warranty_end_date[0]['value'];
    $supply_date       = $node->field_equipment_date[0]['value'];
    $operation_date    = $node->field_equipment_operation_date[0]['value'];
    $warranty_months   = $node->field_equipment_warranty_months[0]['value'];
    $ppm_months        = $node->field_equipment_ppm_months[0]['value'];
    $coding_nid        = $node->field_equipment_coding[0]['nid'];
    $coding_node       = node_load($coding_nid);
    // check if operation and supply date has been already entered
    if (empty($warranty_months)) { 
      return;
    }
    if(!empty($operation_date) & !empty($supply_date)  & empty($warranty_end_date)) {
      $warranty_end_date = strtotime("+$warranty_months month", $operation_date);
      //$equipment_warranty_months = $node->field_equipment_warranty_months[0]['value'];
      $extended_warranty = $node->field_equipment_ext_warranty_mon[0]['value'];
      //$months_of_warranty_and_extended = $extended_warranty + $equipment_warranty_months;
      if(!empty($extended_warranty) && !empty($warranty_end_date)){
        //$warranty_end_date = strtotime("+$months_of_warranty_and_extended month", $warranty_end_date);
         $warranty_end_date = strtotime("+$extended_warranty month", $warranty_end_date);
      }
      _save_ppm_plan_based_warranty($node, $warranty_end_date, $coding_node);
      _save_cal_plan_based_warranty($node, $warranty_end_date, $coding_node);
    }
    elseif (!empty($operation_date) && empty($supply_date)) {
      $warranty_end_date = strtotime("+$warranty_months month", $operation_date);
      //$equipment_warranty_months = $node->field_equipment_warranty_months[0]['value'];
      $extended_warranty = $node->field_equipment_ext_warranty_mon[0]['value'];
      //$months_of_warranty_and_extended = $extended_warranty + $equipment_warranty_months;
      if(!empty($extended_warranty) && !empty($warranty_end_date)){
        //$warranty_end_date = strtotime("+$months_of_warranty_and_extended month", $warranty_end_date);
         $warranty_end_date = strtotime("+$extended_warranty month", $warranty_end_date);
      }
      _save_ppm_plan_based_warranty($node, $warranty_end_date, $coding_node); 
      _save_cal_plan_based_warranty($node, $warranty_end_date, $coding_node);
    }
    // else if operation date is empty and supply date is not 
    // warrantY_end_date = supply_date + warranty_months + 3
    elseif (empty($operation_date) && !empty($supply_date)) {
      $warranty_months   = $warranty_months + 3;
      $warranty_end_date = strtotime("+$warranty_months month", $supply_date); 
      //$equipment_warranty_months = $node->field_equipment_warranty_months[0]['value'];
      $extended_warranty = $node->field_equipment_ext_warranty_mon[0]['value'];
      //$months_of_warranty_and_extended = $extended_warranty + $equipment_warranty_months;
      if(!empty($extended_warranty) && !empty($warranty_end_date)){
        //$warranty_end_date = strtotime("+$months_of_warranty_and_extended month", $warranty_end_date);
         $warranty_end_date = strtotime("+$extended_warranty month", $warranty_end_date);
      }

    }
    elseif (!empty($operation_date) && !empty($supply_date) && !empty($warranty_end_date)) {
      // else if supply_date & operation date & warranty_end_date is entered
      // check if old_warranty_end > calculated warranty (operation_date + warranty_months)
      $new_warranty_date = strtotime("+$warranty_months month", $operation_date);
      if ($new_warranty_date < $warranty_end_date) {
        $warranty_end_date = $new_warranty_date;
      }
      //$equipment_warranty_months = $node->field_equipment_warranty_months[0]['value'];
      $extended_warranty = $node->field_equipment_ext_warranty_mon[0]['value'];
      //$months_of_warranty_and_extended = $extended_warranty + $equipment_warranty_months;
      if(!empty($extended_warranty) && !empty($warranty_end_date)){
        //$warranty_end_date = strtotime("+$months_of_warranty_and_extended month", $warranty_end_date);
         $warranty_end_date = strtotime("+$extended_warranty month", $warranty_end_date);
      }
      _save_ppm_plan_based_warranty($node, $warranty_end_date, $coding_node);
      _save_cal_plan_based_warranty($node, $warranty_end_date, $coding_node);
    }
    // this issue might be needed to be analysed later
    db_query('Update content_type_medical_equipment set field_warranty_end_date_value = %d where nid=%d', $warranty_end_date, $node->nid);
    $node->field_warranty_end_date[0]['value']= $warranty_end_date;

    if (empty($extended_warranty)){
      $extended_warranty = 0;
    }
    $warranty_end_date_without_extended = strtotime("-$extended_warranty month", $warranty_end_date);
    $date = new DateTime();
    $today_date = $date->getTimestamp();
    if ($warranty_end_date_without_extended >= $today_date){
       db_query('Update content_type_medical_equipment set field_equipment_m_contract_stat_value = %d where nid=%d', 0, $node->nid);
      $node->field_equipment_m_contract_stat[0]['value'] = 0;
    }
    elseif($warranty_end_date >= $today_date && !empty($extended_warranty)){
       db_query('Update content_type_medical_equipment set field_equipment_m_contract_stat_value = %d where nid=%d', 1, $node->nid);
      $node->field_equipment_m_contract_stat[0]['value'] = 1;
    }
    else{
      /*$query = db_query('SELECT field_equipment_m_contract_stat_value FROM content_type_medical_equipment WHERE nid = %d', $node->nid);
      if ($result = db_fetch_object($query)){
        $status = $result->field_equipment_m_contract_stat_value;
        db_query('Update content_type_medical_equipment set field_equipment_m_contract_stat_value = %d where nid=%d', $status, $node->nid);
        $node->field_equipment_m_contract_stat[0]['value'] = $status;
      }*/
       
    }

  }
}


/**
 * save new ppm plan based on warranty information
 * @param 
 *   $node -> equipment node
 *   $warranty_end_date -> calculated warranty end date
 *   $coding_node       -> date input item
 */
function _save_ppm_plan_based_warranty($node, $warranty_end_date, $coding_node) {
  if (empty($node->field_ppm_plan_dates[0]['value'])){
    return;
  }
  $ppm_check_query = "select ppm.nid from content_type_ppm ppm inner join content_field_ppm_medical_equips pe using (nid) where pe.field_ppm_medical_equips_nid = %d and ppm.field_ppm_assigned_to_value='CW'" ;
  $results = db_query($ppm_check_query, $node->nid);
  if ($row = db_fetch_array($results)) {
        //drupal_set_message ('This equipment already has a working ppm plan')
  } 
  else {
    if(!empty($node->field_equipment_ppm_months[0]['value']) ) {
      $ppm_plan_node         = (Object) array();
      $ppm_plan_node->type   = 'ppm';
      $ppm_plan_node->uid    = $node->uid;
      $ppm_plan_node->title  = "PPM-". _get_current_year(). '-' ._generate_ppm_code_id();
      $ppm_plan_node->field_ppm_medical_equips[0]['nid']    = $node->nid;
      $ppm_plan_node->field_ppm_next_da[0]['value']         = $node->field_ppm_plan_dates[0]['value'] + (4*3600);
      $ppm_plan_node->field_ppm_next_da[0]['value2']        = $warranty_end_date + (80000);
      $ppm_plan_node->field_ppm_time_period[0]['value']     = $node->field_equipment_ppm_months[0]['value'];
      $ppm_plan_node->field_ppm_hospital[0]['nid']          = $node->field_medical_equip_hospital[0]['nid'];
      $ppm_plan_node->field_ppm_tasks_retrieved[0]['value'] = $coding_node->field_item_ppm_tasks[0]['value'];
      $ppm_plan_node->field_ppm_assigned_to[0]['value']     = 'CW';
      $ppm_plan_node->field_pppm_hospital_person[0]['uid']  = $node->field_equipment_assigned_user[0]['uid'];
      $ppm_plan_node->field_job_client[0]['nid']            = $node->field_equipment_client[0]['nid'];
      _cis_node_save($ppm_plan_node);
     $ppm_link = l(t('View PPM'),('node/' . $ppm_plan_node->nid), array('attributes' => array('target' => '_blank')));
      drupal_set_message($ppm_plan_node->title . " has been saved  " . $ppm_link);
    }
  }
 
}



/**
 * save new ppm plan based on warranty information
 * @param 
 *   $node -> equipment node
 *   $warranty_end_date -> calculated warranty end date
 *   $coding_node       -> date input item
 */
function _save_cal_plan_based_warranty($node, $warranty_end_date, $coding_node) {
  if (empty($node->field_calibration_plan_dates[0]['value'])){
    return;
  }
  $cal_check_query = "select cal.nid from content_type_cal cal inner join content_field_cal_medical_equips ce using (nid) where ce.field_cal_medical_equips_nid = %d and cal.field_calibration_assigned_to_value='CW'" ;
  $results = db_query($cal_check_query, $node->nid);
  if ($row = db_fetch_array($results)) {
    //drupal_set_message ('This equipment already has a working ppm plan')
  } 
  else {
    if(!empty($node->field_equipment_cal_months[0]['value']) ) {
      $cal_plan_node         = (Object) array();
      $cal_plan_node->type   = 'cal';
      $cal_plan_node->uid    = $node->uid;
      $cal_plan_node->title  = "Calibration-". _get_current_year() . "-" . _generate_calibration_code_id();
      $cal_plan_node->field_cal_medical_equips[0]['nid']        = $node->nid;
      $cal_plan_node->field_cal_next_da[0]['value']             = $node->field_calibration_plan_dates[0]['value'] + (4*3600);
      $cal_plan_node->field_cal_next_da[0]['value2']            = $warranty_end_date + (80000);
      $cal_plan_node->field_cal_time_period[0]['value']         = $node->field_equipment_cal_months[0]['value'];
      $cal_plan_node->field_cal_hospital[0]['nid']              = $node->field_medical_equip_hospital[0]['nid'];
      $cal_plan_node->field_cal_tasks_retrieved[0]['value']     = $coding_node->field_item_ppm_tasks[0]['value'];
      $cal_plan_node->field_calibration_assigned_to[0]['value'] = 'CW';
      $cal_plan_node->field_cal_client[0]['nid']                = $node->field_equipment_client[0]['nid'];
      $cal_plan_node->field_cal_hospital_person[0]['uid']       = $node->field_equipment_assigned_user[0]['uid'];
      _cis_node_save($cal_plan_node);
     $cal_link = l(t('View calibration'),('node/' . $cal_plan_node->nid), array('attributes' => array('target' => '_blank')));
      drupal_set_message($cal_plan_node->title . " has been saved  " . $cal_link );

    } 
  } 
}


/**
 * handle log history for different node type
 * @param 
 *  node_type
 */
function _handle_log_history($node, $original_loaded_node) {
  
  $fields_label = array();
  global $user;
  $fields = array();
  $results = array();
  $tests = array();
  foreach ($node as $field_name => $field_array) {
    if (preg_match('/field_/', $field_name)) {
      $fields [] = $field_name;
      if (is_array($node["$field_name"][0]) && is_array($original_loaded_node["$field_name"][0])) {
        $result = array_diff($node["$field_name"][0], $original_loaded_node["$field_name"][0]);
        if(!empty($node["$field_name"]['0']['nid'])) {
          $o_val = $original_loaded_node["$field_name"][0]['nid'];
          $c_val = $node["$field_name"][0]['nid'];
          if ($o_val != $c_val) {
            $results ["$field_name"] =  _get_node_title($c_val);//array ($c_val, $o_val);
          }
        }
        if(!empty($node["$field_name"]['0']['value'])) {
          $o_val = $original_loaded_node["$field_name"][0]['value'];
          $c_val = $node["$field_name"][0]['value'];
          if ($o_val != $c_val) {
            $results ["$field_name"] = $c_val; // array ($c_val, $o_val);
          }
        }
      }
    }
  }
  // log the differencies in the history table
  // change_history table => nid,id,created,operation,userid,username,log_fields
  if (!empty($results)) {
    $sql = "insert into {change_history} values (%d,'', '%d', '%s', %d, '%s','%s')";
    $res = db_query($sql, $node['nid'],strtotime('now'),'update',$user->uid, $user->name, serialize($results));
  }

}



/**
 * return history html table
 * the function will be called in the templates of 
 * Call, work order and Importcode
 */
function _return_history_html_table () {
  // array of dates fields on the system 
  $system_dates_fields_array = array (
    'field_equipment_operation_date',
    'field_equipment_date',
    'field_requested_date',
    'field_warranty_end_date',
    'field_equipment_stop_production',
    'field_equipment_end_of_support',
    'field_job_notify_company_date',
    'field_job_expected_date_of_visit',
    'field_equipment_end_of_support',
    'field_end_of_work',
    'field_job_install_sp_date',
    'field_mcontracts_ppm_start_date',
    'field_mcontracts_cal_start_date',
    'field_ppm_plan_dates',
    'field_calibration_plan_dates',
    'field_instal_date',
    'field_equip_production_date',
    'field_recieve_date'
  );

  $sql = 'select created,operation,userid,username,log_fields from change_history where nid=%d';
  $result = db_query($sql, arg(1));
  $rows = array();
  $log_fields = array();
  while ($row = db_fetch_array($result)) {
  $log_fields_html_table = '<table>';
    $log_fields =  unserialize($row['log_fields']);
    foreach ($log_fields as $key => $val) {

      $content_field = content_fields($key);
      // display date fields in date formate not unixtime
      if (in_array($key, $system_dates_fields_array)) {
        $field_real_val_name = date('d-m-Y', $val);
      }
      // for other fields display
      else {
        $allowed_values = content_allowed_values($content_field);
        $field_real_val_name = $allowed_values[$val];
      }
      
      if (empty($field_real_val_name)) $field_real_val_name = $val;
      $log_fields_html_table.="<tr>";
      $log_fields_html_table.= "<td>". $content_field['widget']['label'] ."</td> <td>"."=> "."$field_real_val_name</td>";
      $log_fields_html_table.= "</tr>";
    }    
    $log_fields_html_table.= "</table>";
    $row['log_fields']     = $log_fields_html_table;//unserialize($row['log_fields']);
    $row['username']       = l($row['username'], 'user/'. $row['userid']);
    $row['created']        = date('d-m-Y : H:m:i', $row['created']);
    $row['role']           = db_result(db_query('select role.name from role inner join users_roles using(rid) where users_roles.uid=%d', $row['userid']));
    unset($row['userid']);
    $rows [] = $row;
  }
  if (empty($rows)) {
    return;
  }
  $html_body .= "
    <div class='portlet box green'>
                     <div class='portlet-title'>
                        <div class='caption'><i class='icon-cogs'></i> ". t('Change History'). "
                                                                                                      </div>
                        <div class='tools'>
                           <a class='collapse' href='javascript:;'></a>
                           <a class='config' data-toggle='modal' href='#portlet-config'></a>
                           <a class='reload' href='javascript:;'></a>
                           <a class='remove' href='javascript:;'></a>
                        </div>
                     </div>
                     <div class='portlet-body lookup-table flip-scroll'>";
  
  $table_body = theme_table(array(t('Date'), t('Operation type'),t('username'), t('Changes on field(s)'), t('role')), $rows, array('table-bordered table-striped table-condensed flip-content'));

  $html_body .= $table_body;
  echo $html_body;

}

/**
 * fill group leader automatic
 * related to Call
 */
function fill_group_leader_automatic_ajax_base_client(){
  $client_nid = arg(4);
  $query = db_query('SELECT role.uid group_leader FROM content_type_client client INNER JOIN content_type_hospital_employee employee ON employee.field_employee_group_value = client.field_client_group_value INNER JOIN node ON node.nid = employee.nid INNER JOIN users_roles role ON node.uid = role.uid WHERE role.rid = 3 AND client.nid = %d', $client_nid);
  if ($result = db_fetch_object($query)){
    $group_leader = $result->group_leader;
  }
  echo $group_leader;
}
