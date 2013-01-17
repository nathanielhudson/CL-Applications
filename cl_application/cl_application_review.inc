<?php
/*
Hokay. So this file contains two forms, and a page. It's a strange way of doing things, but the easiest way I could find.
cl_application_form_bulk_print_form: Displays a table and bulk prints forms.
cl_application_form_review_form: Accept/Rejects forms.
cl_application_list_applications: Our main page, which calls our forms.
*/
require_once('cl_common.inc');

function cl_application_list_applications($u_mode = "new", $u_view = "screen") {
	$mode = check_plain($u_mode);
	$view = check_plain($u_view);
	if ($view!="screen"&&$view!="print"&&$view!="bulk") {
		drupal_not_found();
	}

	if (is_numeric($mode)) {
		//display a single form with id specified by mode
		$result = db_query("SELECT * FROM {cl_application_form} WHERE fid = :fid", array( ':fid' => $mode, ));
		$form = $result->fetchAssoc();
		if (!$form) {
			drupal_not_found();
		}
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

		$team_rows = array();
		$long_rows = array();

		for ($j=0; isset($team_member_fields[$j]); $j++) {
			$team_rows[] = array($team_member_fields_full[$j], check_plain($form['member1'.$team_member_fields[$j]]), check_plain($form['member2'.$team_member_fields[$j]]), check_plain($form['member3'.$team_member_fields[$j]]), check_plain($form['member4'.$team_member_fields[$j]]));
		}

		for ($j=0; isset($long_form_fields[$j]); $j++) {
			$long_rows[] = array($long_form_fields_full[$j], check_plain($form[$long_form_fields[$j]]));
		}

		$output =  array(
   			'item_info' => array(
      			'#items' => array('Company Name: '.check_plain($form['company']),
      			'Date Submitted: '.date('F j, Y, g:i a',check_plain($form['updated'])),
      			'Status: '.check_plain($form['status']),
       			'Source: '.check_plain($form['domain']),
      			'Form ID: '.check_plain($form['fid'])),
     			'#theme' => 'item_list',
    		),
    		'team_table' => array(
      			'#rows' => $team_rows,
      			'#header' => array('Contact Information', 'Team Member #1', 'Team Member #2', 'Team Member #3', 'Team Member #4'),
      			'#theme' => 'table',
    		),
    		'short_answers' => array(
      			'#rows' => $long_rows,
      			'#theme' => 'table',
    		),
  		);

  		if ($form['status'] == 'submitted' && $view == 'screen' && user_access('cl_admin')) {
  			$output[] = drupal_get_form('cl_application_form_review_form', check_plain($form['member1email']), check_plain($form['uid']), $mode);
  		}
  		if ($view == 'screen') {
  			drupal_set_title('Review Page for ' . $form['company']);
			return $output;
  		} elseif ($view == 'bulk') {
			return ("<hr />" . render($output));
  		} elseif ($view == 'print') {
  			cl_application_print_header();
			echo (render($output));
			if (arg(3) != NULL) {
				for ($i=4;arg($i)!=NULL;$i++) {
					echo cl_application_list_applications(arg($i), 'bulk');
				}
			}
			cl_application_print_footer();
  		}
	}
	elseif ($mode=="new"||$mode=="rejected"||$mode=="accepted") {
		// display new, accepted, or rejected applications
  		$output[] = drupal_get_form('cl_application_form_bulk_print_form', $mode, $view);
  		return $output;
	}
	else {
		drupal_not_found();
	}
}

function cl_application_form_bulk_print_form($form, &$form_state, $mode, $view) {
	if ($mode=='new') {$status = 'submitted'; $action = 'Review';}
	elseif ($mode=='rejected') {$status = 'rejected'; $action = 'View';}
	elseif ($mode=='accepted') {$status = 'accepted'; $action = 'View';};
  	drupal_set_title('Listing all ' . $status . ' forms');

	$header = array(
		array('data'=>'Company','field'=>'company'),
		array('data'=>'Domain','field'=>'domain'),
		array('data'=>'Primary Name','field'=>'member1first'),
		array('data'=>'Primary Email','field'=>'member1email'),
		array('data'=>'Date Applied','field'=>'updated','sort'=>'asc'),
		'Action');

	$query = db_select('cl_application_form');
	$query
  		->condition('cl_application_form.status', $status, '=')
  		->fields('cl_application_form', array('updated', 'domain', 'company', 'member1email', 'member1first', 'member1last', 'fid'))
  		->extend('TableSort')
  		->orderByHeader($header);
	$result = $query->execute();
	$form = $result->fetchAssoc();
	$form_rows = array();
	$fids = array();
	while($form){
		//dpm($form);
		$fids[] = check_plain($form['fid']);
		$form_rows[] = array(check_plain($form['company']), check_plain($form['domain']),
			check_plain($form['member1first']).' '.check_plain($form['member1last']),
			check_plain($form['member1email']),
			date('F j, Y',check_plain($form['updated'])),
			'<a href="/ceadmin/review/'.check_plain($form['fid']).'">'.$action.'</a> | <a href="/ceadmin/review/'.
			check_plain($form['fid']).'/print"> Print </a>');
		$form = $result->fetchAssoc();
	}

	$output = array(
		'text' => array(
      		'#type' => 'markup',
      		'#markup' => '<p>View: <a href="'.url('ceadmin/review/new').'">Submitted</a> | <a href="'.
      		url('ceadmin/review/accepted').'">Accepted</a> | <a href="'.url('ceadmin/review/rejected').'">Rejected</a></p>',
      	),
    	'team_table' => array(
      		'#options' => $form_rows,
      		'#header' => $header,
      		'#type' => 'tableselect',
      		'#empty' => 'There\'s nothing to show here yet :(',
    	),
    	'print' => array(
				'#type' => 'submit',
				'#value' => 'Print Selected',
		),
		'#fids' => $fids,
  	);
	return $output;
}

function cl_application_form_bulk_print_form_submit($form, &$form_state) {
	//dpm($form);
	//dpm($form_state);
	$redirect = NULL;
	for($i=0;isset($form['#fids'][$i]);$i++) {
		if ($form_state['input']['team_table'][$i] != NULL){
			if (!$redirect) {
				$redirect = 'ceadmin/review/'.$form['#fids'][$i].'/print/';
			} else {
				$redirect .= $form['#fids'][$i].'/';
			}
		}
	}
	$form_state['redirect'] = $redirect;
}

function cl_application_form_review_form($form, &$form_state, $email, $uid, $fid) {
	$output['action'] = array(
  		'#type' => 'radios',
    			'#options' => array(
     				'accept'   => t('Accept'),
      				'reject' => t('Reject'),
    			),
    			'#title' => t('What do you wish to do with this application?'),
    			'#required' => TRUE,
  			);
  			$output['reject_reason'] = array(
    			'#type' => 'textarea',
    			'#title' => t('Enter a reason for rejection:'),
				'#description' => "This message will be part of the email sent to all members of this team.",
				'#states' => array(
					'visible' => array(':input[name="action"]' => array('value' => 'reject'),),
					'required' => array(':input[name="action"]' => array('value' => 'reject'),),
    			),
  			);
  			$output['reject_message'] = array(
    			'#type' => 'textarea',
    			'#title' => t('Customize rejection message:'),
    			'#default_value' => variable_get('cl_default_app_reject'),
				'#description' => "This message will be sent to all members of this team. \":reason:\" will be replaced with your reason for rejection. You do not need to change this text.",
				'#states' => array(
					'visible' => array(':input[name="action"]' => array('value' => 'reject'),),
					'required' => array(':input[name="action"]' => array('value' => 'reject'),),
    			),
  			);
  			$output['accept_message'] = array(
    			'#type' => 'textarea',
    			'#title' => t('Customize aceptance message:'),
    			'#default_value' => variable_get('cl_default_app_accept'),
				'#description' => "This message will be sent to all members of this team. You do not need to change this text.",
				'#states' => array(
					'visible' => array(':input[name="action"]' => array('value' => 'accept'),),
					'required' => array(':input[name="action"]' => array('value' => 'accept'),),
    			),
  			);
  			$output['submit'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
			);
			$output['#uid'] = $uid;
			$output['#fid'] = $fid;
			$output['#email'] = $email;
	return $output;
}

function cl_application_form_review_form_validate($form, &$form_state) {
	if (($form_state['input']['action'] == 'reject') && ($form_state['input']['reject_reason'] == '')) {
		form_set_error('reject_reason', t('You need to enter a reason to reject this form! Please enter a suitable message.'));
	}
}

function cl_application_form_review_form_submit($form, &$form_state) {
	if (!is_numeric($form['#fid'])) {
		drupal_not_found();
	}
	$domain = cl_get_domain();
	db_update('cl_application_form')
  		->fields(array(
    		'status' => $form_state['input']['action'] .'ed',
  		)) ->condition('fid', $form['#fid'], '=') ->execute();
  	if ($form_state['input']['action'] == 'reject') {
  		$reject_message = str_replace(':reason:', $form_state['input']['reject_reason'], $form_state['input']['reject_message']);
		cl_set_progress($form['#uid'], 'none');
		$hid = cl_new_history_event('Application Rejected', $form['#uid'], '/ceadmin/review/'.$form['#fid'],
			$reject_message, $color = 'red');
		cl_send_email($form['#uid'], $reject_message, 'Your application on '.$domain['sitename'].' has been declined', 'application', $hid);
	} else if ($form_state['input']['action'] == 'accept') {
		cl_advancl_section($form['#uid']);
		$hid = cl_new_history_event('Application Accepted', $form['#uid'], '/ceadmin/review/' . $form['#fid'],
			$form_state['input']['accept_message'], $color = 'green');
		cl_send_email($form['#uid'], $form_state['input']['accept_message'],
			'Your application on '.$domain['sitename'].' has been accepted!', 'application', $hid);

	}
}
?>