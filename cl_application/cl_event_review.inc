<?php
require_once('cl_common.inc');

function ltw_category_description($i) {
	switch ($i) {
		case 1:
			return 'Company\'s market offer solves a problem that is specific to identifiable target cutomers.';
		break;
		case 2:
			return 'Company\'s market offer delivers more value to target customers when compared to alternatives and alternatives are well understood.';
		break;
		case 3:
			return 'Company will provide good value to those channels and partners that help bring it\'s market offer to customers.';
		break;
		case 4:
			return 'Company can generate at least 6 good knowledgeable jobs in Canada\'s capital region over the next three years.';
		break;
		case 5:
			return 'Company canhas a sound financial plan and founders have a sound plan to feed their families untill a company generates sufficient cash to pay themselves.';
		break;
		case 6:
			return 'Company knows how to leverage the resources available in Canada\'s captal region to grow.';
		break;
		case 7:
			return 'Core team has what it takes to achieve it\'s objectives over the next three years.';
		break;
		default:
			return '';
		break;
	}
}

function ltw_category_names($i) {
	switch ($i) {
		case 1:
			return 'Customer Value';
		break;
		case 2:
			return 'Competitive';
		break;
		case 3:
			return 'Partner Value';
		break;
		case 4:
			return 'Jobs';
		break;
		case 5:
			return 'Financial';
		break;
		case 6:
			return 'Foundation';
		break;
		case 7:
			return 'Team';
		break;
		default:
			return '';
		break;
	}
}


function cl_event_review_form($form, &$form_state, $event_id, $signup_id) {
	$node = node_load($event_id);
	$signup = db_select('cl_signup', 's')->fields('s')->condition('s.signup_id', $signup_id)->execute()->fetchAssoc();
	$team = db_select('cl_team', 't')->fields('t')->condition('t.uid', $signup['uid'])->execute()->fetchAssoc();
	drupal_set_title('Review Form for '.$team['company']);
	$names = '';
	for($i = 1; $i<=4; $i++){
		if ($team['member'.$i.'first'] != '' && $team['member'.$i.'last'] != '') {
			$names.=($names==''?'':', ').$team['member'.$i.'first'].' '.$team['member'.$i.'last'];
		}
	}
	if ($signup['reviewed']!=''){
		$history_event = db_select('cl_history', 'h')->fields('h')->condition('h.hid', $signup['reviewed'])->execute()->fetchAssoc();
		return array('info' => array(
			'#type' => 'markup',
			'#markup' => '<p>Event Name: '.$node->title.'<br />'.
			'Date: '.date("F j, Y",strtotime($node->field_event_time['und'][0]['value'])).' at '.check_plain($signup['slot']).'<br />'.
			'Panel: '.check_plain($signup['panel']).'<br />'.
			'Team: '.check_plain($names).'<br />'
			.'</p>'
			),
			'history' => array(
	      		'#title' => check_plain($history_event['title']),
	      		'#url' => check_plain($history_event['url']),
	      		'#hid' => check_plain($history_event['hid']),
	      		'#message' => check_markup($history_event['message']),
	      		'#date' => 'Reviewed on: '.check_plain(date('F j, Y, g:i a',$history_event['date'])),
	      		'#color' => check_plain($history_event['color']),
	     		'#theme' => 'cl_application_progress',
		  	),
		);
	}
	$agree_disagree = array(
		NULL => NULL,
		1 => t('1 - Strongly Disagree'),
		2 => t('2'),
		3 => t('3 - Agree'),
		4 => t('4'),
		5 => t('5 - Strongly Agree'),
	);
	$show_on_check = array(
		'visible' => array(':input[name="show_long"]' => array('checked' => TRUE),),
		'required' => array(':input[name="show_long"]' => array('checked' => TRUE),),
	);
	return array(
		'#uid' => $signup['uid'],
		'#signup_id' => $signup_id,
		'#event_id' => $event_id,
		'#event_name' => $node->title,
		'info' => array(
			'#type' => 'markup',
			'#markup' => '<p>Event Name: '.$node->title.'<br />'.
			'Date: '.date("F j, Y",strtotime($node->field_event_time['und'][0]['value'])).' at '.check_plain($signup['slot']).'<br />'.
			'Panel: '.check_plain($signup['panel']).'<br />'.
			'Team: '.check_plain($names).'<br />'
			.'</p>'
		),
		'show_long' => array(
			'#type' => 'checkbox', 
			'#title' => t('Use expanded feedback?'),
		),
		'expanded_feedback' => array(
			'#type' => 'fieldset', 
			'#title' => t('Expanded Feedback'), 
			'#collapsible' => TRUE, 
			'#collapsed' => FALSE,
			'#states' => $show_on_check,
			'#visible' => FALSE,
			'#tree' => TRUE,
			'customer_value' => array(
				'#type' => 'select',
				'#title' => t('1 - Customer Value'),
				'#options' => $agree_disagree,
				'#required' => FALSE,
				'#description' => ltw_category_description(1),
			),
			'competitive' => array(
				'#type' => 'select',
				'#title' => t('2 - Competitive'),
				'#options' => $agree_disagree,
				'#required' => FALSE,
				'#description' => ltw_category_description(2),
			),
			'partner_value' => array(
				'#type' => 'select',
				'#title' => t('3 - Partner Value'),
				'#options' => $agree_disagree,
				'#required' => FALSE,
				'#description' => ltw_category_description(3),
			),
			'jobs' => array(
				'#type' => 'select',
				'#title' => t('4 - Jobs'),
				'#options' => $agree_disagree,
				'#required' => FALSE,
				'#description' => ltw_category_description(4),
			),
			'financial' => array(
				'#type' => 'select',
				'#title' => t('5 - Financial'),
				'#options' => $agree_disagree,
				'#required' => FALSE,
				'#description' => ltw_category_description(5),
			),
			'foundation' => array(
				'#type' => 'select',
				'#title' => t('6 - Foundation'),
				'#options' => $agree_disagree,
				'#required' => FALSE,
				'#description' => ltw_category_description(6),
			),
			'team' => array(
				'#type' => 'select',
				'#title' => t('7 - Team'),
				'#options' => $agree_disagree,
				'#required' => FALSE,
				'#description' => ltw_category_description(7),
			),
		),
		'action' => array(
			'#type' => 'radios',
			'#options' => array(
				'green'   => t('Green'),
				'yellow' => t('Yellow'),
				'red' => t('Red'),
			),
				'#title' => t('How do you want to mark this application?'),
				'#description' => "Only green gains an advancement to the next section. Yellow and red keep the person in the same section.",
				'#required' => TRUE,
			),
			'feedback' => array(
				'#type' => 'textarea',
	    	'#title' => t('Feedback:'),
			'#description' => "Please provide feedback to the team below.",
	    	'#required' => TRUE,
			/*'#states' => array(
				'visible' => array(':input[name="action"]' => array('value' => 'reject'),),
				'required' => array(':input[name="action"]' => array('value' => 'reject'),),
   			),*/
		),
		'save' => array(
			'#type' => 'submit',
			'#value' => 'Submit',
		),
	);
}

function cl_event_review_form_validate($form, &$form_state) {
	if ($form_state['input']['show_long'] == 1) {
		foreach($form_state['complete form']['expanded_feedback'] as $key => $item) {
			if ($key[0] != '#' && $item['#value']==NULL) {
				form_set_error('expanded_feedback]['.$key, '"'.$item['#title'].'" is required');
			}
		}
	}
}


function cl_event_review_form_submit($form, &$form_state) {
	//cl_new_history_event($title, $uid, $url, $message, $color = 'gray')
	//cl_send_email($uid, $message, $subject, $history_item=NULL)
	$expanded_review = '';
	$text_opener = '';
	if ($form_state['input']['action'] == 'green') {
		$text_opener = 'Congratulations! You have been marked as green and have progressed to the next section. Your results are as follows:'."\n\n";
	} elseif ($form_state['input']['action'] == 'yellow') {
		$text_opener = 'You have been marked as yellow. Your opportunity needs to be improved; however, the likelihood that it can be improved is good. Your results are as follows:'."\n\n"; //TODO: approve this text
	} elseif ($form_state['input']['action'] == 'red') {
		$text_opener = 'You have been marked as red. Your opportunity is not currently ready for the program. Your results are as follows:'."\n\n";
	}
	if ($form_state['input']['show_long'] == 1) {
		$expanded_review .= 'The items below are evaluated out of five, with 1 meaning "strongly disagree" and 5 meaning "strongly agree"'."\n";
		foreach($form_state['complete form']['expanded_feedback'] as $key => $item) {
			if ($key[0] != '#') {
				$expanded_review .= $item['#title']."\n".$item['#description']."\nEvaluation: ".check_plain($item['#value']."\n\n");
			}
		}
	}
	//dpm($expanded_review);
	$history_item = cl_new_history_event($form['#event_name'], $form['#uid'], '', $text_opener.$expanded_review.$form_state['input']['feedback'], $form_state['input']['action']);
	db_update('cl_signup')
		->fields(array(
			'reviewed' => $history_item,
			'expanded_review' => serialize($form_state['input']['expanded_feedback']),
			'review_color' => $form_state['input']['action'],
		))
		->condition('signup_id', $form['#signup_id'])
		->execute();
	if ($form_state['input']['action'] == 'green') {
		cl_advancl_section($form['#uid']);
	} else {
		cl_set_progress($form['#uid']);
	}
	cl_send_email($form['#uid'], $text_opener.$expanded_review.$form_state['input']['feedback'], 'Your results from '.$form['#event_name'], 'application', $history_item);
	drupal_goto('ceadmin/event/review/'.$form['#event_id']);
}
?>