<?php
require_once('cl_common.inc');
require_once('cl_tasks_admin_forms.inc');

function cl_list_p3_users () {
	//$GLOBALS["user"]->uid
	$domain = cl_get_domain();

	$query = db_select('cl_team', 't');
	//$query->leftJoin('cl_task_assigned', 'k', 't.uid = k.uid');
	$query = $query->condition('t.domain', $domain['domain_id'])
		->condition('t.section', 'phase3')
		->fields('t', array('uid','company', 'member1first', 'member1last', 'member2first', 'member2last', 'member3first', 'member3last', 'member4first', 'member4last'))
		->execute();
	
	
	
	$result = $query->fetchassoc();
	$rows = array();
	while($result){
		$members = array();
		$members[] = $result['member1first'].' '.$result['member1last'];
		if ($result['member2first']) {
			$members[] = $result['member2first'].' '.$result['member2last'];
		}
		if ($result['member3first']) {
			$members[] = $result['member3first'].' '.$result['member3last'];
		}
		if ($result['member4first']) {
			$members[] = $result['member4first'].' '.$result['member4last'];
		}

		
		$rows[] = array(
			'<a href="./phase3/'.$result['uid'].'">'.$result['company'].'</a>',
			'<ul><li>'.implode ('</li><li>', $members).'</li></ul>',
			'<a href="phase3/'.$result['uid'].'">View Tasks</a>'
		);
		$result = $query->fetchassoc();
	}
	return array(
		'help_text' => array(
			'#markup' => '<h2>Opportunity Admin List</h2><p><!--TODO: Help Text Here--></p><hr />',
		),
		'table' => array(
			'#theme' => 'table',
			'#empty' => 'No '.variable_get('cl_final_section_name').' teams exist yet!',
			'#header' => array('Company', 'Members', 'Tasks'),
			'#rows' => $rows,
		),
		'form_new' => drupal_get_form('cl_add_new_task_form', 'global'),
		//'form' => drupal_get_form('cl_add_task_form', 2),
	);
}

function cl_show_p3_user ($uid) {
	//$GLOBALS["user"]->uid
	if (!is_numeric($uid)) {
		drupal_not_found();
	}
	
	$domain = cl_get_domain();

	$team = db_select('cl_team', 't')
		->condition('t.domain', $domain['domain_id'])
		->condition('t.uid', $uid)
		->fields('t', array('company', 'member1first', 'member1last', 'member2first', 'member2last', 'member3first', 'member3last', 'member4first', 'member4last'))
		->execute()
		->fetchassoc();
	
	$tasks_query = db_select('cl_task_assigned', 'a')
		->condition('a.domain', $domain['domain_id'])
		->condition('a.uid', array($uid, 0));
		//->condition('m.con_uid', $uid)
	$tasks_query->leftJoin('cl_task', 't', 't.task_id = a.task_id');
	$tasks_query = $tasks_query->fields('a')->fields('t')->execute();
	
	$tasks = $tasks_query->fetchassoc();
	$tasks_text = '<h2>Assigned Opportunities:</h2>';
	while($tasks) {
		/*$date_text = '';
		if ($tasks['deadline']) {
			$date_text = '<small class="duedate">To be completed by '.date('F j, Y, g:i a', $tasks['deadline']).'</small>';
		}*/
		$tasks_text .= '<div class="task"><h3>'.check_plain($tasks['title']).'</h3><p>'.check_plain($tasks['description']).'</p></div>';
		/*$messages_text = '';
		$messages_query = db_select('cl_task_message', 't')
			->condition('t.task_id', $tasks['task_id'])
			->condition('t.con_uid', $uid)
			//->condition('m.con_uid', $uid)
			->fields('t', array('uid', 'description', 'state'))
			->execute();
		$messages = $messages_query->fetchassoc();
		while($messages) {
			
			$poster=user_load($messages['uid']);
			$tasks_text .='<div class="message message-'.($messages['uid']==$uid?'author':'admin').'"><h4>'.$poster->name.'</h4><p>'.check_plain($messages['description']).'</p></div>';
			$messages = $messages_query->fetchassoc();
		}*/
		$tasks = $tasks_query->fetchassoc();
	}
	$team_text = '<h2>'.$team['company'].'</h2><p>Team Members:</p>';
	
	$members = array();
	$members[] = $team['member1first'].' '.$team['member1last'];
	if ($team['member2first']) {
		$members[] = $team['member2first'].' '.$team['member2last'];
	}
	if ($team['member3first']) {
		$members[] = $team['member3first'].' '.$team['member3last'];
	}
	if ($team['member4first']) {
		$members[] = $team['member4first'].' '.$team['member4last'];
	}
	$team_text .= '<ul><li>'.implode ('</li><li>', $members).'</li></ul>';

	
	return array(
		'team_text' => array(
			'#markup' => $team_text,
		),
		'task_text' => array(
			'#markup' => $tasks_text,
		),
		'form_header' => array(
			'#markup' => '<h2>Add Tasks</h2>',
		),
		'form_existing' => drupal_get_form('cl_add_existing_task_form', $uid),
		'form_new' => drupal_get_form('cl_add_new_task_form', $uid),
		//'form' => drupal_get_form('cl_add_task_form', 2),
	);
}
?>
