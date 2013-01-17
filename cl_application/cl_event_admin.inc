<?php
require_once('cl_common.inc');

function cl_event_admin() {
    $rows = array();
    $node_query = db_select('node', 'n')
	    ->condition('n.type', 'cl_event')
	    ->orderBy('created', 'DESC')
	    ->fields('n', array('nid', 'title'))
	    ->execute();
	$node_result = $node_query->fetchAssoc();
    while($node_result) {
        //So there are about a billion queries in here. There's probably a better way to do this, but since it's not a high-traffic page, I'm going to assume it won't matter.
        $signedup = db_select('cl_signup', 's')
	        ->condition('s.nid', $node_result['nid'])
	        ->countQuery()
	        ->execute()
	        ->fetchField();
        $unasssigned = db_select('cl_signup', 's')
	        ->condition('s.nid', $node_result['nid'])
	        ->condition(db_or()
	            ->condition('s.slot', '')
	            ->condition('s.panel', ''))
	        ->countQuery()
	        ->execute()
	        ->fetchField();
	    $notnotified = db_select('cl_signup', 's')
	        ->condition('s.nid', $node_result['nid'])
	        ->condition('s.notified', '')
	        ->countQuery()
	        ->execute()
	        ->fetchField();
	    $notreviewed = db_select('cl_signup', 's')
	        ->condition('s.nid', $node_result['nid'])
	        ->condition('s.reviewed', '')
	        ->countQuery()
	        ->execute()
	        ->fetchField();
	    $actions = '<a href="/ceadmin/event/assign/'.$node_result['nid'].'">Assign</a> | <a href="/ceadmin/event/review/'.$node_result['nid'].'">Review</a>';
        $rows[]=array($node_result['title'], $signedup, $unasssigned, $notnotified, $notreviewed, $actions);
	    $node_result = $node_query->fetchAssoc();
    }
    return array(
		'help_text' => array(
      		'#type' => 'markup',
      		'#markup' => '<p>TODO: Help text here</p>',
      	),
		'events_table' => array(
					'#rows' => $rows,
					'#header' => array('Event Name', 'Signed up', 'Not Assigned', 'Not Notified', 'Not Reviewed', 'Actions'),
					'#theme' => 'table',
					'#empty' => 'No events yet. You can add one through the link below.',
        ),
		'new_link' => array(
      		'#type' => 'markup',
      		'#markup' => '<p><a href="/node/add/ce-event">Add New Event</a></p>',
      	),
    );
}

function cl_event_review_table($url_arg) {	
    if ((!isset($url_arg)) || (!is_numeric($url_arg))) {
        drupal_not_found();
    }
		drupal_add_css(drupal_get_path('module','cl_application') . '/cl_event_admin.css', array('type'=>'file'));
		drupal_add_js(drupal_get_path('module','cl_application') . '/cl_event_table_print.js', array('type'=>'file'));
		drupal_add_library('system', 'effects');
		drupal_add_library('system', 'ui.button');
		
    $nid = $url_arg;
    $event_node = node_load($nid);
    if ((!$event_node) || $event_node -> type != 'cl_event') {
        drupal_not_found();
    }
    $query = db_select('cl_signup', 's')
	    ->condition('s.nid', $nid);
	$query->leftJoin('cl_team', 't', 's.uid = t.uid');
	$query = $query
        ->fields('s')
	    ->fields('t', array('company', 'domain', 'member1title', 'member1first', 'member1last'))
	    ->execute();
	$query_result = $query->fetchAssoc();

	$output = array(
		'text' => array(
      		'#type' => 'markup',
      		'#markup' => '<p class="noprint">Please click on a company to input results from the interview or view results from companies that have already been reviewed. Companies that have not been reviewed are marked gray, and those which have are coloured according to their results.</p>',
      	),
    	'time_table' => array(
    	    '#theme' => 'table',
    	    '#rows' => array(),
			'#header' => array('Time'),
    	),
  	);

  	foreach($event_node->field_section_names['und'] as $item) {
        $output['time_table']['#header'][] = $item['safe_value'];
        $output['time_table']['#rows'][$item['safe_value']] = array();
    }

	$times_with_invisibles = explode("\n", $event_node->field_time_slots['und'][0]['safe_value']);
	foreach($times_with_invisibles as &$time) {
		$clean_time = preg_replace('/[\x00-\x1F\x80-\xFF]/', '', $time);
		$output['time_table']['#rows'][$clean_time] = array();
		$output['time_table']['#rows'][$clean_time]['Time'] = $clean_time;
		
		foreach($event_node->field_section_names['und'] as $item) {
			$output['time_table']['#rows'][$clean_time][$item['safe_value']] = '';
		}
	}
	$invalid_signups = 0;
	$invalid_signups_text = '';
	while($query_result) {
		if (isset($output['time_table']['#rows'][check_plain($query_result['slot'])][check_plain($query_result['panel'])])) {
			$class = ($query_result['review_color']=='')?'':' cl_event_item_'.check_plain($query_result['review_color']);
			$link='#';
			if($query_result['review_color']==''){
				$link = $url_arg.'/'.$query_result['signup_id'];
			}else{
				//$link = '/ceadmin/progress/'.$query_result['uid'].'/'.$query_result['reviewed'];
				$link = $url_arg.'/'.$query_result['signup_id'];
			}		
			$company_text = '<div class="cl_event_item'.$class.'"><a href="'.$link.'">'.check_plain($query_result['company']).' - '.
				check_plain($query_result['member1first'].' '.$query_result['member1last']).'</a></div>';
			$output['time_table']['#rows'][check_plain($query_result['slot'])][check_plain($query_result['panel'])] .= $company_text;
		} else {
			$invalid_signups += 1;
			$company_text = '<li>'.check_plain($query_result['company']).' - '.
				check_plain($query_result['member1first'].' '.$query_result['member1last']).'</li>';
			
			$invalid_signups_text .= $company_text;
		}

		$query_result = $query->fetchAssoc();
	}

    if ($invalid_signups > 0) {
        $output['bad_entries'] = array(
    	    '#type' => 'markup',
    	    '#markup' => '<h2>'.$invalid_signups.($invalid_signups==1?' Entry Has Not Been Assigned a Valid Slot!':'Entries Have Not Been Assigned Valid Slots!').'</h2>
    	    <p>Please assign these signups valid slots on the <a href="../assign/'.$url_arg.'">assignment page for this event</a>. The signups which have not been assigned are listed below:</p><ul>'.$invalid_signups_text.'</ul>',
    	);
    }

    return $output;
}
?>