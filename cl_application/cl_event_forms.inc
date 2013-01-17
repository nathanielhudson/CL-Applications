<?php
require_once('cl_common.inc');

function cl_interview_signup_form($form, &$form_state, $variables) {
    $form['#nid'] = $variables['nid'];
    $form['#node_title'] = $variables['title'];
	$form['signup'] = array(
        '#type' => 'fieldset',
        '#title' => t('Interview Signup'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
    );
	$form['signup']['time'] = array(
        '#type' => 'radios',
        '#options' => array(),
        //'#default_value' => $category['selected'],
    );
    $registrations = db_select('cl_signup', 'c')
            ->fields('c', array('slot'))
            ->condition('nid', $variables['nid'])
            ->execute()->fetchCol();
    $user_registration = db_select('cl_signup', 'c')
            ->fields('c', array('slot'))
            ->condition('nid', $variables['nid'])
            ->condition('uid', $GLOBALS["user"]->uid)
            ->execute()->fetchField();

    $no_slots_available = true;
    foreach ($variables['times'] as $time) {
        $clean_time = preg_replace('/[\x00-\x1F\x80-\xFF]/', '', $time); //if we don't strip invisible junk here the form errors out on submit.
        if (strpos($time, ' - ') === false) {
            $available_slots=count($variables['groups']);
            foreach ($registrations as $registration) {
                if ($registration == $clean_time) {  $available_slots -= 1; }
            }
            if ($available_slots > 0 || $user_registration==$clean_time) {
                $form['signup']['time']['#options'][$clean_time] = $clean_time . '  -  '. $available_slots . ' slot' . ($available_slots==1?'':'s') . ' remaining.';
                $form['signup']['time'][$clean_time] = array('#disabled' => $available_slots<=0, '#description' => $user_registration==$clean_time?'You are currently registered in this time slot.':'');
            }
            //$form['signup']['time'][$clean_time] = array('#disabled' => $available_slots<=0, '#description' => $user_registration==$clean_time?'You are currently registered in this time slot.':'');
        } else {
            //$empty=0;
						$available_slots=0;
            //$form['signup']['time']['#options'][$clean_time] = $clean_time;
            //$form['signup']['time'][$clean_time] = array('#disabled' => TRUE);
        }
        $no_slots_available = $no_slots_available && $available_slots<=0;
    }
    $form['signup']['submit'] = array('#type' => 'submit', '#value' => t($user_registration?'Change my registration':'Sign Up for this Event'), '#disabled' => $no_slots_available && (!$user_registration));
    if ($user_registration) {
        $form['signup']['time']['#options']['cancel'] = 'Cancel My Registration';
        $form['signup']['time']['#description'] = t('You have already registered for this event. If you wish to change your registration, please select a time.');
    } elseif ($no_slots_available) {
        $form['signup']['time']['#description'] = t('We\'re sorry, but this event is full.');
    } else {
        $form['signup']['time']['#description'] = t('Please select which time you would like to sign up for.');
    }

	return $form;
}

function cl_interview_signup_form_submit($form, &$form_state) {
    if ($form_state['input']['time'] === NULL) {
        drupal_set_message('Nothing selected. Your time was not changed.');
    } elseif ($form_state['input']['time'] == 'cancel') {
        db_delete('cl_signup')
            ->condition('nid', $form['#nid'])
            ->condition('uid', $GLOBALS["user"]->uid)
            ->execute();
        cl_new_history_event('Registration Canceled', $GLOBALS["user"]->uid, '/node/'.$form['#nid'], 'You have canceled your registration for '.$form['#node_title']);
    } else {
        db_merge('cl_signup')
            ->key(array('nid' => $form['#nid'], 'uid' => $GLOBALS["user"]->uid))
            ->fields(array(
                'slot' => $form_state['input']['time'],
            ))
            ->execute();
        cl_new_history_event('Thank You for Registering', $GLOBALS["user"]->uid, '/node/'.$form['#nid'], 'You have registered for '.$form['#node_title']);
    }
}

function cl_assessment_signup_form($form, &$form_state, $variables) {
    $registrations = db_select('cl_signup', 'c')
            ->condition('nid', $variables['nid'])
            ->countQuery()->execute()->fetchField();
    $user_registration = db_select('cl_signup', 'c')
            ->condition('nid', $variables['nid'])
            ->condition('uid', $GLOBALS["user"]->uid)
            ->countQuery()->execute()->fetchField();
    $valid_times = 0;
    foreach ($variables['times'] as $time) {
        if (strpos($time, ' - ') === false) {
            $valid_times += 1;
        }
    }
    $slots_available = ($valid_times*count($variables['groups'])) - $registrations;

    $form['#nid'] = $variables['nid'];
    $form['#node_title'] = $variables['title'];
	$form['signup'] = array(
        '#type' => 'fieldset',
        '#title' => t('Assessment Signup'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#description' => 'This event has ' . $slots_available . ' slots available.' . ($user_registration?' You are already signed up for this event.':''),
    );

    $form['signup']['submit'] = array('#type' => 'submit',
        '#value' => t($user_registration?'Cancel my registration':'Sign Up for this Event'),
        '#disabled' => $slots_available<=0 && (!$user_registration));

	return $form;
}

function cl_assessment_signup_form_submit($form, &$form_state) {
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
}

?>