<?php
require_once('cl_common.inc');

function cl_add_new_task_form ($form, &$form_state, $target) {
	if ($target == 'global') {
		$title = 'Add New Task to All Teams';
		$button_text = 'Add New Task to All Teams';
	} elseif (is_numeric($target)) {
		$team_name = db_select('cl_team', 't')
			->condition('t.uid', $target)
			->fields('t', array('company'))
			->execute()
			->fetchfield();
		if (!$team_name) {
			drupal_not_found();
		}
		$title = 'Add New Task to '.$team_name;
		$button_text = 'Add New Task to '.$team_name;
	} else {
		drupal_not_found();
	}
	return array(
		'task_fieldset' => array(
			'#type' => 'fieldset',
			'#title' => $title,
			'#collapsible' => TRUE, 
			'#collapsed' => TRUE,
			'title' => array(
				'#type' => 'textfield',
				'#title' => 'Title',
				'#maxlength' => 128,
				'#required' => TRUE,
			),
			'description' => array(
				'#type' => 'textarea',
				'#title' => 'Description',
				'#required' => TRUE,
			),
			/*'has_deadline' => array(
				'#type' => 'checkbox',
				'#title' => 'Does this task have a date it must be completed by?',
			),
			'deadline' => array(
				'#type' => 'date_select',
				'#title' => 'Deadline',
				'#default_value' => date('Y-m-d').' 00:00:00',
				'#required' => TRUE,
				'#states' => array(
		      // Hide the settings when the cancel notify checkbox is disabled.
		      'invisible' => array(
		       ':input[name="has_deadline"]' => array('checked' => FALSE),
		      ),
		    ),
			),*/
			'submit' => array(
				'#type' => 'submit',
				'#value' => $button_text,
			),
		)
	);
}

function cl_add_new_task_form_submit($form, &$form_state) {
	$domain = cl_get_domain();
	
	$target = $form_state['build_info']['args'][0];
	if (!($target === 'global' || is_numeric($target))) {
		drupal_not_found();
		return;
	}
	if ($target === 'global') {
		$query = db_select('cl_team', 't');
		$query = $query
			->condition('t.domain', $domain['domain_id'])
			->condition('t.section', 'phase3')
			->fields('t', array('uid'))
			->execute()
			->fetchcol();
		$uids = $query; //UID 0 here stores an task that's assigned to everyone
	} else {
		$uids = array($target);
	}
/*	$deadline = NULL;
	if ($form_state['input']['has_deadline']) {
		$deadline = mktime(
			$form_state['input']['deadline']['hour'],
			$form_state['input']['deadline']['minute'],
			0, // no seconds on the input widget, so just store 0 seconds.
			$form_state['input']['deadline']['month'],
			$form_state['input']['deadline']['day'],
			$form_state['input']['deadline']['year']
		);
	}*/
	
	
	$task_id = db_insert('cl_task')
		->fields(array(
			'domain' => $domain['domain_id'],
			//'uid' => $uid,
			'title' => $form_state['input']['title'],
			'description' => $form_state['input']['description'],
			//'deadline' => $deadline,
			//'state' => 'incomplete',
		))->execute();

		foreach ($uids as $uid) {
			db_insert('cl_task_assigned')
				->fields(array(
					'domain' => $domain['domain_id'],
					'uid' => $uid,
					'task_id' => $task_id,
				))->execute();
			cl_new_history_event('New Opportunity Assigned', $target, 'ce/opportunities', 'An administrator has assigned you a new opportunity!');
		}
}

function cl_add_existing_task_form ($form, &$form_state, $target) {
	$domain = cl_get_domain();
	if ($target == 'global') {
		$title = 'Add Existing Tasks to All Teams';
		$button_text = 'Add Tasks to All Teams';
	} elseif (is_numeric($target)) {
		$team_name = db_select('cl_team', 't')
			->condition('t.uid', $target)
			->fields('t', array('company'))
			->execute()
			->fetchfield();
		if (!$team_name) {
			drupal_not_found();
		}
		$title = 'Add Existing Task to Team '.$team_name;
		$button_text = 'Add Existing Task to '.$team_name;
	} else {
		drupal_not_found();
	}
	
	$tasks_available = array();
	$tasks_query = db_select('cl_task', 't')
		->condition('t.domain', $domain['domain_id'])
		->fields('t', array('task_id', 'title'))
		->execute();
	$tasks_row = $tasks_query->fetchassoc();
	while ($tasks_row) {
		$tasks_available[$tasks_row['task_id']] = $tasks_row['title'];
		$tasks_row = $tasks_query->fetchassoc();
	}
	
	return array(
		'task_fieldset' => array(
			'#type' => 'fieldset',
			'#title' => $title,
			'#collapsible' => TRUE, 
			'#collapsed' => TRUE,
			'task' => array(
				'#title' => 'Available Tasks',
				'#type' => 'checkboxes',
				'#options' => $tasks_available,
			),
			/*'has_deadline' => array(
				'#type' => 'checkbox',
				'#title' => 'Does this task have a date it must be completed by?',
			),
			'deadline' => array(
				'#type' => 'date_select',
				'#title' => 'Deadline',
				'#default_value' => date('Y-m-d').' 00:00:00',
				'#required' => TRUE,
				'#states' => array(
		      // Hide the settings when the cancel notify checkbox is disabled.
		      'invisible' => array(
		       ':input[name="has_deadline"]' => array('checked' => FALSE),
		      ),
		    ),
			),*/
			'submit' => array(
				'#type' => 'submit',
				'#value' => $button_text,
			),
		)
	);
}
function cl_add_existing_task_form_submit ($form, &$form_state) {
	$domain = cl_get_domain();
	$target = $form_state['build_info']['args'][0];
	//dpm($form_state);
	foreach($form_state['input']['task'] as $task_id) {
		if ($task_id === NULL) {
			continue;
		}
		//Check if task is already assigned
		$tasks_query = db_select('cl_task_assigned', 't')
			->condition('t.domain', $domain['domain_id'])
			->condition('t.uid', $target)
			->condition('t.task_id', $task_id)
			->fields('t')
			->execute()
			->fetchcol();

		if ($tasks_query == array()) {
			db_insert('cl_task_assigned')
				->fields(array(
					'domain' => $domain['domain_id'],
					'uid' => $target,
					'task_id' => $task_id,
					))->execute();
					cl_new_history_event('New Opportunity Assigned', $target, 'ce/opportunities', 'An administrator has assigned you a new opportunity!');
		} else {
			drupal_set_message('Task '.$task_id.' was already assigned to user '.$target.', and was not assigned a second time.');
		}
	}
}

?>