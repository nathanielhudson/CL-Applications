<?php
require_once('cl_common.inc');

function cl_application_first_form($form, &$form_state) {
 	$team_member_fields = array('email', 'title', 'first', 'last', 'phone', 'street', 'city', 'postal', 'country', 'province', 'program', 'year');
 	$team_member_fields_full = array('Email Address', 'Salutation', 'First Name', 'Last Name', 'Phone Number', 'Street Address', 'City (Home)', 'Postal Code (Home)', 'Country (Home)', 'Province (Home)', 'Degree or Program', 'Year of Study');
 	$long_form_fields = array('experience', 'skills', 'product', 'customers', 'problem', 'userbenefits', 'orgbenefits', 'differ', 'team');
 	$long_form_fields_full = array('Experience', 'Skills, Relationships, Experience, Resouces', 'Product or Service', 'Target Customers', 'Problem Solved', 'User Benefits', 'Organizational Benefits', 'Key Differentiators', 'Core Team');
	$long_form_fields_descriptions = array('Please describe your experience you have as it relates to the business you wish to start or grow.',
		'Please describe why you think you can launch and grow a successful business.',
		'Please describe what the product or service your business will profitably sell.',
		'Please describe who the target customers are and why did you select them.',
		'Please describe what customer pain or problem your product or service will address. Provide concrete examples of customer pain or problem.',
		'Please describe benefits the users of your product or service will obtain.',
		'Please describe what benefits the organizations that employ these users will obtain.',
		'Please describe the points of difference of your product or service for which target customers are willing to pay.',
		'Please describe the core team currently working on your opportunity or the core team you wish to recruit to work on your opportunity.'
	);
	$domain = cl_get_domain();
	$result = db_query("SELECT * FROM {cl_application_form} WHERE uid = :uid AND domain = :domain ORDER BY updated DESC LIMIT 1", array( ':uid' => $GLOBALS["user"]->uid, ':domain' => $domain['domain_id']));
	$defaults = $result->fetchAssoc();

	$form['description'] = array(
		'#type' => 'item',
		'#title' => t('Application Form'),
	);
	$form['company'] = array(
		'#type' => 'textfield',
		'#title' => t('Company Name'),
		'#required' => TRUE,
		'#maxlength'=> 255,
		//'#default_value' => "...",
		//'#description' => "Please enter your first name.",
		//'#size' => 20,
		//'#maxlength' => 20,
	);
	$form['member_1'] = array(
		'#type' => 'fieldset',
		'#title' => t('Team Member #1'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);
	$form['member_2'] = array(
		'#type' => 'fieldset',
		'#title' => t('Team Member #2'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);
	$form['member_3'] = array(
		'#type' => 'fieldset',
		'#title' => t('Team Member #3'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);
	$form['member_4'] = array(
		'#type' => 'fieldset',
		'#title' => t('Team Member #4'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);
	if (isset($defaults['company'])) {
		$form['company']['#default_value'] = $defaults['company'];
	}
	for ($i = 1; $i <= 4; $i++) {
		for ($j=0; isset($team_member_fields[$j]); $j++) {
			$form['member_'.($i)]['member'.$i.$team_member_fields[$j]] = array(
				'#type' => 'textfield',
				'#title' => t($team_member_fields_full[$j]),
				'#required' => ($i == 1),
				'#maxlength'=> 255,
			);
			//dpm($defaults['member'.$i.$team_member_fields[$j]]);
			if (isset($defaults['member'.$i.$team_member_fields[$j]])) {
				$form['member_'.($i)]['member'.$i.$team_member_fields[$j]]['#default_value'] = $defaults['member'.$i.$team_member_fields[$j]];
			}
		}
	}
	for ($j=0; isset($long_form_fields[$j]); $j++) {
		$form[$long_form_fields[$j]] = array(
			'#type' => 'textarea',
			'#description' => t($long_form_fields_descriptions[$j]." (Maximum 200 words)"),
			'#title' => t($long_form_fields_full[$j]),
			'#required' => TRUE,
			'#maxlength'=> 10000,
		);
		if (isset($defaults[$long_form_fields[$j]])) {
			$form[$long_form_fields[$j]]['#default_value'] = $defaults[$long_form_fields[$j]];
		}
	}
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Submit',
	);
	$form['draft'] = array(
		'#type' => 'submit',
		'#value' => 'Save Draft',
	);
	return $form;
}


/**
 * Implements hook_form_FORM_ID_alter()
 */
function cl_application_form_cl_application_first_form_alter(&$form, $form_state, $form_id)  {
	//dpm($form);
	//dpm($form_state);
	//dpm($form_id);
	//dpm(db_query("SELECT nid FROM {node}"));

	//$domain=cl_get_domain();
	//if ($domain['domain_id'] != "localhost" && $domain['domain_id'] != "Lead To Win") {
	//	drupal_goto('http://localhost.org/'.request_uri());
	//}

	if (!$GLOBALS["user"]->uid) {
		drupal_set_message(t("Before you can apply, you must first create an account. You'll use this account to track your progress."), 'warning');
		drupal_set_message(t("If you have already created an account on one of our partner sites, that account will also work here."), 'warning');
		drupal_goto('user/register');
	}
	$progress = cl_get_progress($GLOBALS["user"]->uid);
	if (($progress == NULL) || ($progress == array('section' => 'application', 'progress' => 'none'))) {
		return;
	} elseif ($progress == array('section' => 'application', 'progress' => 'submitted')) {
		drupal_set_message(t("You have already submitted an application. Please wait for an administrator to review your form."), 'warning');
		drupal_goto('');
		return;
	} 
	drupal_set_message(t("You do not need to submit another application right now. Please review your progress to see your next steps."), 'warning');
	drupal_goto('');	
}

/**
 * Validation function for
 */
function cl_application_first_form_validate($form, &$form_state) {
	if ($form_state['values']['op'] == 'Save Draft') {
		//If we're saving a draft, dismiss all form errors and all relevant drupal error messages
		//This lets users save incomplete forms as drafts
		form_clear_error();
		$messages = drupal_get_messages('error'); //clears errors. wut.
		if (isset($messages['error'])) {
			foreach ($messages['error'] as $msg) {
				if (stristr($msg, "required")) continue;
				//drupal_set_message($msg, 'error');
				form_set_error('#submit', $msg);
			}
		}
	} else {
 		$long_form_fields = array('experience', 'skills', 'product', 'customers', 'problem', 'userbenefits', 'orgbenefits', 'differ', 'team');
 		$team_member_fields_full = array('Email Address', 'Salutation', 'First Name', 'Last Name', 'Phone Number', 'Street Address', 'City (Home)', 'Postal Code (Home)', 'Country (Home)', 'Province (Home)', 'Degree or Program', 'Year of Study');
 		foreach ($long_form_fields as $field) {
 			if (str_word_count($form_state['values'][$field]) > 210) {
 				//210, because I'll give 'em a margin.
				form_set_error($field, 'This answer must be shorter than 200 words.');
 			}
 		}
	}
}

/**
 * Submit function
 *
 */
function cl_application_first_form_submit($form, &$form_state) {
	//dpm($form_state);

 	$team_member_fields = array('email', 'title', 'first', 'last', 'phone', 'street', 'city', 'postal', 'country', 'province', 'program', 'year');
 	$long_form_fields = array('experience', 'skills', 'product', 'customers', 'problem', 'userbenefits', 'orgbenefits', 'differ', 'team');

	if ($form_state['values']['op'] == 'Submit') {
		$operation = "submitted";
	} elseif ($form_state['values']['op'] == 'Save Draft') {
		$operation = "draft";
	} else {
		drupal_set_message(t('Invalid Operation! Saving as Draft.'), 'error');
		$operation = "draft";
	}
	$domain = cl_get_domain();
	$formQuery = array(
		'company' => $form_state['values']['company'],
		'uid' => $GLOBALS["user"]->uid,
		'updated' => REQUEST_TIME,
		'status' => $operation,
		'domain' => $domain['domain_id'],
	);

	for ($i = 1; $i <= 4; $i++) {
		foreach ($team_member_fields as $field) {
			$formQuery['member'.$i.$field] = $form_state['values']['member'.$i.$field];
		}
	}
	foreach ($long_form_fields as $field) {
		$formQuery[$field] = $form_state['values'][$field];
	}

	// Merge Query: http://drupal.org/node/310085
	// Updates an existing draft if it already exists, if not, creates a new entry.
	db_merge('cl_application_form')
	    ->key(array('uid' => $GLOBALS["user"]->uid, 'status' => 'draft'))
	    ->fields($formQuery)
	    ->execute();
    $fid = db_select('cl_application_form')
        ->fields('cl_application_form', array('fid'))
        ->condition('uid', $GLOBALS["user"]->uid)
        ->condition('status', $operation)
        ->execute()
        ->fetchField();

	if ($operation == "draft") {
		drupal_set_message(t("Draft Saved."));
	} elseif ($operation == "submitted") {
		$teamQuery = array(
			'uid' => $GLOBALS["user"]->uid,
			'company' => $form_state['values']['company'],
			'section' => 'application',
			'progress' => 'submitted',
			'domain' => $domain['domain_id'],
		);
		for ($i = 1; $i <= 4; $i++) {
			foreach ($team_member_fields as $field) {
				$teamQuery['member'.$i.$field] = $form_state['values']['member'.$i.$field];
			}
		}
		//Create/update a new entry in the cl_team (NOT cl_application_form!) table.
		db_merge('cl_team')->key(array('uid' => $GLOBALS["user"]->uid))->fields($teamQuery)->execute();
		drupal_set_message(t("Application Submitted"));

		$form_state['redirect'] = array('ce/progress/'.cl_new_history_event('Submitted Application', $GLOBALS["user"]->uid, '/ce/review/'.$fid, 'Thank you for submitting an application! Your application is now pending review. Please check back here for feedback.', 'gray'));

	}
}
?>