<?php
require_once('cl_common.inc');

function cl_show_history ($uid=NULL,$target=NULL) {
	if (!$GLOBALS["user"]->uid) {
		drupal_set_message(t("Before you can view your progress, you must first create an account. You'll use this account to track your progress."), 'warning');
		drupal_set_message(t("If you have already created an account on one of our partner sites, that account will also work here."), 'warning');
		drupal_goto('user/register');
	}
	if ($uid == NULL) {
		$uid = $GLOBALS["user"]->uid;
	} else {
		$user = user_load($uid);
		drupal_set_title('Viewing '.$user->name.'\'s history');
	}
	drupal_add_js(array('cl_application' => array('target' => $target)), 'setting');
	drupal_add_js(drupal_get_path('module','cl_application') . '/jquery.scrollTo-1.4.2-min.js', array('type'=>'file'));
	drupal_add_js(drupal_get_path('module','cl_application') . '/cl_history.js', array('type'=>'file'));

	$progress = cl_get_progress($uid);
	$task = cl_get_section_name_description($progress['section']);
	$output[] = array(
      	'#markup' => '<h2>Current Section: '.check_plain($task['name']).'</h2><p>'.check_markup($task['description']).'</p><hr />',
	);
	$domain = cl_get_domain();
	$history_events_query = db_select('cl_history', 'c')
		->condition('c.uid', $uid)
		->condition('c.domain', $domain['domain_id'])
  		->fields('c')
  		->orderBy('c.date', 'DESC')
		->execute();
	$history_events = $history_events_query -> fetchAssoc();
	while ($history_events) {
		$output[] = array(
      		'#title' => check_plain($history_events['title']),
      		'#url' => check_plain($history_events['url']),
      		'#hid' => check_plain($history_events['hid']),
      		'#message' => check_markup($history_events['message']),
      		'#date' => check_plain(date('F j, Y, g:i a',$history_events['date'])),
      		'#color' => check_plain($history_events['color']),
     		'#theme' => 'cl_application_progress',
	  	);
		$history_events = $history_events_query->fetchAssoc();
	}
  	return $output;
  }
?>