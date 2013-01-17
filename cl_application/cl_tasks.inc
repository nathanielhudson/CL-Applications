<?php
require_once('cl_common.inc');

function cl_show_tasks () {
	$domain = cl_get_domain();
	
	$tasks_query = db_select('cl_task_assigned', 'a')
		->condition('a.domain', $domain['domain_id'])
		->condition('a.uid', array($GLOBALS["user"]->uid, 0));
		//->condition('m.con_uid', $uid)
	$tasks_query->leftJoin('cl_task', 't', 't.task_id = a.task_id');
	$tasks_query = $tasks_query->fields('a')->fields('t')->execute();
	
	$tasks = $tasks_query->fetchassoc();
	$tasks_text = '<h2>Assigned Opportunities:</h2>';
	while($tasks) {
		$tasks_text .= '<div class="task"><h3>'.check_plain($tasks['title']).'</h3><p>'.check_plain($tasks['description']).'</p></div>';
		$tasks = $tasks_query->fetchassoc();
	}
	
	$output[] = array(
		'#markup' => '<h2>'.variable_get('cl_final_section_name').'</h2>'.variable_get('cl_phase_3_description').$tasks_text,
	);
	
	$domain = cl_get_domain();
	return $output;
}
?>