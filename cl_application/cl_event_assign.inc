<?php
require_once('cl_common.inc');

function cl_event_assignment_form($form, &$form_state, $url_arg) {
    if ((!isset($url_arg)) || (!is_numeric($url_arg))) {
        drupal_not_found();
    }
    $nid = $url_arg;
    $event_node = node_load($nid);
    if ((!$event_node) || $event_node -> type != 'cl_event') {
        drupal_not_found();
    }

    $event_type = cl_get_section_type($event_node->field_event_section['und'][0]['value']);
	$query = db_select('cl_signup', 's')
	    ->condition('s.nid', $nid);
	$query->leftJoin('cl_team', 't', 's.uid = t.uid');
	$query = $query->fields('s') //, array('signup_id', 'slot', 'panel')
	    ->fields('t', array('company', 'domain', 'member1title', 'member1first', 'member1last'))
	    ->execute();
	$query_result = $query->fetchAssoc();

    $output = array(
		'text' => array(
      		'#type' => 'markup',
      		'#markup' => '<p><!--Description Text Goes Here--></p>',
      	),
    	'time_table' => array(
    	    '#tree' => true,
    	    '#panels' => array(),
			'#times' => array(),
            '#event_type' => $event_type,
    	),
    	'save' => array(
				'#type' => 'submit',
				'#value' => 'Save Changes',
		),
		'signup_ids' => array(),
  	);

    foreach($event_node->field_section_names['und'] as $item) {
        $output['time_table']['#panels'][$item['safe_value']] = $item['safe_value'];
    }

	$times_with_invisibles = explode("\n", $event_node->field_time_slots['und'][0]['safe_value']);
    $available_times = array(''=>'');
    foreach($times_with_invisibles as &$time) {
        $clean_time = preg_replace('/[\x00-\x1F\x80-\xFF]/', '', $time);
        $output['time_table']['#times'][$clean_time] = $clean_time;
        if (strpos($clean_time, ' - ') === false)
            $available_times[$clean_time] = $clean_time;
    }

	while($query_result){
		$output['time_table'][$query_result['signup_id']] = array(
		    'company' => array('#markup' => check_plain($query_result['company'])),
		    //'domain' => array('#markup' => check_plain($query_result['domain'])),
		    'name' => array('#markup' => check_plain($query_result['member1first'].' '.$query_result['member1last'])),
		    'slot' => array(
                '#type' => 'select',
                '#options' => $available_times,
                '#default_value' => $query_result['slot'],
                '#attributes' => array('class'=>array('slot_selector')),
            ),
            'panel' => array(
                '#type' => 'select',
                '#options' => $output['time_table']['#panels'],
                '#default_value' => $query_result['panel'],
                '#attributes' => array('class'=>array('panel_selector')),
            ),
		    '#slot_data' => check_plain($query_result['slot']),
		    '#panel_data' => check_plain($query_result['panel']),
        );

        $output['time_table'][$query_result['signup_id']]['panel']['#options'] = array(''=>'') + $output['time_table'][$query_result['signup_id']]['panel']['#options'];
		$query_result = $query->fetchAssoc();
	}


	return $output;
}

function theme_cl_event_assignment_form(&$variables) {
	$form = $variables['form'];

	drupal_add_css(drupal_get_path('module','cl_application') . '/cl_event_admin.css', array('type'=>'file'));
	drupal_add_library('system', 'effects');
	drupal_add_library('system', 'ui.button');
	drupal_add_library('system', 'ui.dialog');
	drupal_add_js(array('cl_application' => array('event_type' => $form['time_table']['#event_type'])), 'setting');
	drupal_add_js(drupal_get_path('module','cl_application') . '/cl_event_assign.js', array('type'=>'file'));

    $header = array('Time' => 'Time') + $form['time_table']['#panels'] + array('Unassigned' => 'Unassigned');


    $table_arr = array();
    foreach($form['time_table']['#times'] as $time) {
        $table_arr[$time] = array();
        foreach($header as $panel) {
            if ($panel != 'Time') {
                $table_arr[$time][$panel] = '';
            } else {
                $table_arr[$time]['Time'] = $time;
            }
        }
        unset($panel);
    }
    unset($time);

    $table_arr['Unassigned'] = array();
    foreach($header as $panel) {
        if ($panel != 'Time') {
            $table_arr['Unassigned'][$panel] = '';
        } else {
            $table_arr['Unassigned']['Time'] = 'Not Set';
        }
    }
    unset($panel);
    $table_arr['Unassigned']['Unassigned'] = '';

    foreach (element_children($form['time_table']) as $id) {
        $time = ($form['time_table'][$id]['#slot_data']=='')?'Unassigned':$form['time_table'][$id]['#slot_data'];
        $panel = ($form['time_table'][$id]['#panel_data']=='')?'Unassigned':$form['time_table'][$id]['#panel_data'];
        $table_arr[$time][$panel] .= '<div class="cl_event_item">'.
            drupal_render($form['time_table'][$id]['company']).' - '.drupal_render($form['time_table'][$id]['name']).
            '<div class="event_controls">'.drupal_render($form['time_table'][$id]['slot']).drupal_render($form['time_table'][$id]['panel']).'</div>'.
            '</div>';
    }

    $output = drupal_render($form['help']);
    $output .= theme('table', array('header' => $header, 'rows' => $table_arr, 'attributes' => array('id' => 'cl_event_admin_table')));
    $output .= drupal_render_children($form);
    return $output;
}

function cl_event_assignment_form_validate($form, &$form_state) {
    $occupied_slots = array();
		if (!isset($form_state['input']['time_table'])) {
			return;
		}
		
    foreach ($form_state['input']['time_table'] as $id => $item) {
        if ($item['slot'] != '' && $item['panel'] != '') {
            if (isset($occupied_slots[$item['slot'] . ' %%% ' . $item['panel']])) {
                form_set_error('time_table]['.$id, 'You have multiple items occupying the same slot and panel.');
                form_set_error('time_table]['.$occupied_slots[$item['slot'] . ' %%% ' . $item['panel']],'You have multiple items occupying the same slot and panel.');
            } else {
                $occupied_slots[$item['slot'] . ' %%% ' . $item['panel']] = $id;
            }
        }
    }
}

function cl_event_assignment_form_submit($form, &$form_state) {

    foreach ($form_state['input']['time_table'] as $id => $item) {
         $up = db_update('cl_signup')
            ->fields(array(
                'slot' => $item['slot'],
                'panel' => $item['panel'],
            ))
            ->condition('signup_id', $id, '=')
            ->execute();
        //Notify user here...
    }


    /*
    db_update('cl_signup')
        ->fields(array(
            'uid' => 5,
            'status' => 1,
        ))
        ->condition('nid', $form['#nid'])
        ->condition('uid', $GLOBALS["user"]->uid)
        ->execute();
    */
    /*
    if ($form_state['values']['op'] == 'Cancel my registration') {
        db_delete('cl_signup')
            ->condition('nid', $form['#nid'])
            ->condition('uid', $GLOBALS["user"]->uid)
            ->execute();
        cl_new_history_event('Registration Canceled', $GLOBALS["user"]->uid, '/node/'.$form['#nid'], 'You have canceled your registration for '.$form['#node_title']);
    } elseif ($form_state['values']['op'] == 'Sign Up for this Event') {
        db_merge('cl_signup') ->key(array('nid' => $form['#nid'], 'uid' => $GLOBALS["user"]->uid))->execute();
        cl_new_history_event('Thank You for Registering', $GLOBALS["user"]->uid, '/node/'.$form['#nid'], 'You have registered for '.$form['#node_title']);
    }
    */
}
?>