<?php
	//$result = db_select('cl_sections', 'c')->condition('c.domain', 'localhost', '=')->fields('c', array('sid', 'name', 'weight'))->orderBy('weight', 'ASC')->execute();

function cl_section_order_form($form, &$form_state) {
	$domain = cl_get_domain();
	$form['dragable_items']['#tree'] = TRUE;
	//$result = db_query('SELECT id, name, description, weight FROM {tabledrag_example} WHERE id < 8 ORDER BY weight ASC');
	$result = db_select('cl_sections', 'c')
		->condition('c.domain', $domain['domain_id'], '=')
		->fields('c', array('sid', 'name', 'type', 'weight', 'description'))
		->orderBy('weight', 'ASC')
		->execute();
	$form['help'] = array('#markup'=>'<p>Here you can change the number and order of gates, or "sections" that a team must pass through in order to reach phase 3. Items higher on the list must be completed before items at the bottom. Although not shown, the list always begins with the Application Form, and ends with Phase 3.</p><p>The number in the "Teams" column shows the number of teams currently in that section. Sections with teams currently in them <em>cannot</em> be deleted or have their type changed, although any other action is still allowed.</p><p>You can rearrange sections by dragging them. The description field can be resized for easier editing.</p>');
	foreach ($result as $item) {
		$count = db_select('cl_team', 'c')
			->condition('c.section', $item->sid, '=')
			->countQuery()
			->execute()
			->fetchField();
    	$form['dragable_items'][$item->sid] = array(
    	'count' => array('#markup' => $count),
    	'type' => array(
    		'#type' => 'select',
       		'#options' => array(
       			'' => '',
          		'interview' => t('Interview'),
         		'assessment' => t('Assessment'),
       		),
       		'#title' => 'Type',
			'#disabled' => $count > 0,
    		'#default_value' => check_plain($item->type),
		),
		'delete' => array(
			'#type' => 'submit',
			'#value' => t('Delete Section'),
			'#name' => 'delete' . $item->sid,
			'#disabled' => $count > 0,
			'#return_value' => 'delete' . $item->sid,
		),
		'name' => array(
        	'#type' => 'textfield',
        	'#default_value' => check_plain($item->name),
        	'#size' => 40,
        	'#title' => 'Name',
        	'#maxlength' => 255,
      	),
      	'description' => array(
        	'#type' => 'textarea',
        	'#default_value' => check_plain($item->description),
        	'#rows' => 1,
        	'#title' => 'Description',
      	),
      	'weight' => array(
        	'#type' => 'weight',
        	'#title' => t('Weight'),
        	'#default_value' => ($item->weight),
        	'#delta' => 30,
        	'#title-display' => 'invisible',
      	),
		);
	}
	if (isset($form_state['storage']['newsection'])) {
		$form['dragable_items']['new'] = $form_state['storage']['newsection'];
	}
	$form['actions'] = array('#type' => 'actions');
	$form['actions']['submit'] = array( '#type' => 'submit', '#value' => t('Save Changes'), '#return_value' => 'save');
	$form['actions']['addnew'] = array( '#type' => 'submit', '#value' => t('Add New Section'), '#return_value' => 'add new');
	return $form;
}

function theme_cl_section_order_form($variables) {
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form['dragable_items']) as $id) {
    $form['dragable_items'][$id]['weight']['#attributes']['class'] = array('dragable-item-weight');
    $rows[] = array(
      'data' => array(
      	'',
      	drupal_render($form['dragable_items'][$id]['count']),
        drupal_render($form['dragable_items'][$id]['name']) . drupal_render($form['dragable_items'][$id]['description']),
        drupal_render($form['dragable_items'][$id]['type']) . drupal_render($form['dragable_items'][$id]['delete']),
        drupal_render($form['dragable_items'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  $header = array('',t('Teams'),t('Name and Description'), t('Type'), t('Weight'));
  $table_id = 'example-items-table';
  $output = drupal_render($form['help']);
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'empty' => 'No sections yet! Click the "Add New Section" button to add one!', 'attributes' => array('id' => $table_id)));
  $output .= drupal_render_children($form);
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'dragable-item-weight');
  return $output;
}

function cl_section_order_form_validate($form, &$form_state) {
	if (isset($form_state['values']['dragable_items'])) {
		foreach ($form_state['values']['dragable_items'] as $id => $item) {
			if(substr($form_state['triggering_element']['#return_value'], 0, 6)=="delete" &&
			substr($form_state['triggering_element']['#return_value'], 6)==$id) {break;}
			if($item['type'] == ''){form_set_error('dragable_items]['.$id.'][type', 'Please select a type of section.');}
			if($item['name'] == ''){form_set_error('dragable_items]['.$id.'][name', 'Please enter a name for this section.');}
			if($item['description'] == ''){form_set_error('dragable_items]['.$id.'][description', 'Please enter a description for this section.');}
		}
	}
}

function cl_section_order_form_submit($form, &$form_state) {
	//removes duplicates and spaces from array.
	if (isset($form_state['values']['dragable_items'])) {
		$weightmap = array();
		foreach ($form_state['values']['dragable_items'] as $item) {
			$weightmap[] = $item['weight'];
		}
		sort($weightmap);
		foreach($weightmap as $newweight => $oldweight) {
			foreach ($form_state['values']['dragable_items'] as $id => &$item) {
				if (!isset($item['mappedweight']) && $item['weight'] == $oldweight) {
					$item['mappedweight'] = $newweight;
					break;
				}
			}
			unset($id);
  			unset($item);
		}
	}

	if($form_state['triggering_element']['#return_value']=="add new") {
		$form_state['storage']['newsection']= array(
    	'type' => array(
    		'#type' => 'select',
       		'#options' => array(
       			'' => '',
          		'interview' => t('Interview'),
         		'assessment' => t('Assessment'),
       		),
       		'#title' => 'Type',
		),
		'delete' => array(
			'#type' => 'submit',
			'#value' => t('Delete Section'),
			'#name' => 'deletenew',
			'#return_value' => 'deletenew',
		),
		'name' => array(
        	'#type' => 'textfield',
        	'#size' => 40,
        	'#title' => 'Name',
        	'#maxlength' => 255,
      	),
      	'description' => array(
        	'#type' => 'textarea',
        	'#title' => 'Description',
        	'#rows' => 2,
      	),
      	'weight' => array(
        	'#type' => 'weight',
        	'#title' => t('Weight'),
        	'#default_value' => 30,
        	'#delta' => 30,
        	'#title-display' => 'invisible',
      	),
		);
		$form_state['rebuild'] = TRUE;
	}

	if(substr($form_state['triggering_element']['#return_value'], 0, 6)=="delete" &&
	substr($form_state['triggering_element']['#return_value'], 6) == 'new') {
		unset($form_state['values']['dragable_items']['new']);
	}

	if($form_state['triggering_element']['#return_value']=="save" || TRUE) {
  		if (isset($form_state['values']['dragable_items'])) {
			foreach ($form_state['values']['dragable_items'] as $id => $item) {
				if ($id != 'new') {
					$up = db_update('cl_sections')
						->fields(array(
						    'name' => $item['name'],
						    'type' => $item['type'],
						    'description' => $item['description'],
						    'weight' => $item['mappedweight']
						))
						->condition('sid', $id, '=')
						->execute();
					//dpm(' name:' . $item['name'] . ' weight:'. $item['mappedweight'] . ' affected:'.$up);
				} else
				{
					$domain = cl_get_domain();
					db_insert('cl_sections')
						->fields(array('name' => $item['name'],'type' => $item['type'],
						'description' => $item['description'], 'weight' => $item['weight'],
						'domain' => $domain['domain_id']))
						->execute();

				}
			}
		}
  	}

  	if(substr($form_state['triggering_element']['#return_value'], 0, 6)=="delete" &&
  	is_numeric(substr($form_state['triggering_element']['#return_value'], 6))) {
		db_delete('cl_sections')->condition('sid', substr($form_state['triggering_element']['#return_value'], 6))->execute();
	}
}
