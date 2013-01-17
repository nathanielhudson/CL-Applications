<?php

function cl_get_domain() {
	//Just so we're not dependant on domain
	if (module_exists('domain')) {
		return domain_get_domain();
	} else {
		return array('domain_id' => 1);
	}
}

function cl_new_history_event($title, $uid, $url, $message, $color = 'gray') {
	$domain = cl_get_domain();
	 $hid = db_insert('cl_history')->fields(array(
		'title' => $title,
    	'uid' => $uid,
     	'url' => $url,
     	'message' => $message,
     	'color' => $color,
		'domain' => $domain['domain_id'],
  		'date' => REQUEST_TIME,
  	))->execute();
  	return $hid;
}

function cl_set_progress($uid, $progress='none', $section='') {
    assert($uid != 0);
	//yeah, it's a bad function name, as set_progress can set both section and progress. WHOOPS.
	if ($section=='') {
		db_update('cl_team')->fields(array('progress' => $progress))->condition('uid', $uid)->execute();
		watchdog(WATCHDOG_INFO, 'Setting user '.$uid.'\'s section progress to '.$progress);
	} else {
		db_update('cl_team')->fields(array('progress' => $progress, 'section' => $section))->condition('uid', $uid)->execute();
		watchdog(WATCHDOG_INFO, 'Setting user'.$uid.'\'s section id to '.$section.', and section progress to '.$progress);
	}
}

function cl_get_progress($uid) {
	//Ditto to above. This function returns both section and progress as an array.
	$domain = cl_get_domain();
	if (!isset($domain['domain_id'])) {
		return -1; //For some reason domain_get_domain returns incomplete results sometimes :( - currently no good way to handle this.
		//I've switched everything over to using the domain_id rather than the machine_name. It seems to work better now.
	}
	return db_select('','cl_team')->fields('cl_team', array('section', 'progress'))->condition('uid', $uid)->condition('domain', $domain['domain_id'])->execute()->fetchAssoc();
}

function cl_get_section_type($sid) {
	if ($sid=='application'){return 'application';}
	if ($sid=='phase3'){return 'phase3';}
	$type = db_select('cl_sections')
		->condition('sid', $sid, '=')
  		->fields('cl_sections', array('type'))
		->execute()
		->fetchAssoc();
	return $type['type'];
}

function cl_get_section_name_description($sid) {
	if ($sid=='application') {
		return array('name' => 'Application', 'description' => variable_get('cl_application_description'));
	} elseif ($sid==NULL) {
		return array('name' => 'Application', 'description' => variable_get('cl_application_description'));
	} elseif ($sid=='phase3') {
		return array('name' => variable_get('cl_final_section_name'), 'description' => variable_get('cl_phase_3_description'));
	}
	$result = db_select('cl_sections')
		->condition('sid', $sid, '=')
  		->fields('cl_sections', array('name', 'description'))
		->execute()
		->fetchAssoc();
	return $result;
}

function cl_get_section_weight($sid) {
	//dpm($sid);
	if ($sid=='application'){return -9999;}
	if ($sid=='phase3'){return 9999;}
	$weight=db_select('cl_sections')->fields('cl_sections',array('weight'))->condition('sid', $sid)->execute()->fetchAssoc();
	return $weight['weight'];
}

function cl_advancl_section($uid) {
	$progress=cl_get_progress($uid);
	$domain=cl_get_domain();
	$weight=cl_get_section_weight($progress['section']);
	$next=db_select('cl_sections')
		->fields('cl_sections',array('sid'))
		->condition('domain', $domain['domain_id'], '=')
		->condition('weight', $weight, '>')
		->orderBy('weight', 'ASC')
		->range(0, 1)
		->execute()
		->fetchAssoc();
	//dpm($next['sid']);
	if ($next['sid'] != NULL) {
		cl_set_progress($uid, 'none', $next['sid']);
	} else {
		//if you've got nowhere left to go, send 'em to phase 3
		cl_set_progress($uid, 'none', 'phase3');
	}
}

function cl_application_mail($key, &$message, $params) {
	global $user;
	$options = array(
		'langcode' => $message['language']->language,
	);

	switch ($key) {
	case 'cl_send_email':
		$message['subject'] = check_plain($params['subject']);
		$message['body'][] = check_plain($params['message']);
      break;
  }
}

function cl_send_email($uid, $message, $subject, $key, $history_item=NULL) {
	
	$recip_query = db_select('cl_team', 'c')
	  ->condition('c.uid', $uid)
	  ->fields('c', array('member1email', 'member2email', 'member3email', 'member4email'))
	  ->execute()
		->fetchAssoc();
	$recipients = implode(array_filter($recip_query), ',');
	$domain = cl_get_domain();
	$signature = "\n-".variable_get('cl_program_name');
	if ($history_item) {
		$signature.="\n\nYou can view this event, along with the rest if your progress at "
			.$domain['path']."ce/progress/".$history_item;
	}
	dpm("EMAIL SERVICE MESSAGE\nTO:".$recipients."\nSUBJECT:".$subject."\nBODY:".$message.$signature);
	
	drupal_mail('cl_application', $key, $recipients, user_preferred_language($uid),
		array('message'=>$message.$signature, 'subject'=>$subject));
}

function cl_application_print_header() {
	$filename = drupal_get_path('module','cl_application') . "/print.css";
	$handle = fopen($filename, "r");
	$contents = fread($handle, filesize($filename));
	fclose($handle);
	echo ('<html><head><style>' . $contents . '</style></head><body onload="window.print(); window.history.back();">');
}

function cl_application_print_footer() {
	echo ('</body></html>');
}

function cl_time_slots_explode($text) {
	$time_components = explode(':', $text);
	$output = array();
	if (isset($time_components[1])) {
		$time_components[1] = explode(' - ', $time_components[1]);
	}
	if (isset($time_components[0])) {$output[] = preg_replace('/[\x00-\x1F\x80-\xFF]/', '', $time_components[0]);}
	if (isset($time_components[1]) && !is_array($time_components[1])) {$output[] = preg_replace('/[\x00-\x1F\x80-\xFF]/', '', $time_components[1]);}
	if (isset($time_components[1][0])) {$output[] = preg_replace('/[\x00-\x1F\x80-\xFF]/', '', $time_components[1][0]);}
	if (isset($time_components[1][1])) {$output[] = preg_replace('/[\x00-\x1F\x80-\xFF]/', '', $time_components[1][1]);}
	return $output;
}


?>