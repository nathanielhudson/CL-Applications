<?php
require_once('cl_common.inc');

function cl_admin_settings_form($form, &$form_state) {
   return array(
		'info' => array(
			'#type' => 'markup',
			'#markup' => '<p>TODO: Help Text...</p>'
		),
		'program_name' => array(
			'#type' => 'textfield',
			'#title' => 'Program Name',
			'#description' => 'The name of this program. Shown on emails and in the application interface.',
			'#default_value' => variable_get('cl_program_name'),
			'#maxlength' => 128,
			'#required' => TRUE,
		),
		'lock_status' => array(
			'#type' => 'select',
			'#title' => t('Domain Behaviour'),
			'#required' => TRUE,
			'#options' => array(
				'closed' => 'Locked',
				//'redirect' => 'Redirect',
				'open' => 'Open',
			),
			'#default_value' => variable_get('cl_lock_status'),
			'#description' => t('Set whether this system is accessible on this domain.'),
		),
		'redirect_target' => array(
			'#type' => 'hidden', //change to textfield to enable
			'#title' => 'Redirect Target',
			'#description' => 'If "Domain Behaviour" is set to redirect, where to redirect to?',
			'#default_value' => variable_get('cl_redirect_target'),
			'#maxlength' => 128,
			'#required' => FALSE,
		),
		'application_feedback' => array(
			'#type' => 'fieldset',
			'#title' => 'Application Feedback Settings',
			'#collapsible' => TRUE, 
			'#collapsed' => FALSE,
			'default_app_reject' => array(
				'#type' => 'textarea',
				'#title' => 'Default Application Rejection Message',
				'#description' => 'Default email for rejected application forms.',
				'#default_value' => variable_get('cl_default_app_reject'),
				'#required' => TRUE,
			),
			'default_app_accept' => array(
				'#type' => 'textarea',
				'#title' => 'Default Application Acceptance Message',
				'#description' => 'Default email for accepted application forms.',
				'#default_value' => variable_get('cl_default_app_accept'),
				'#required' => TRUE,
			),
		),
		'section_settings' => array(
			'#type' => 'fieldset',
			'#title' => 'Section Display Settings',
			'#collapsible' => TRUE, 
			'#collapsed' => FALSE,
			'final_section_name' => array(
				'#type' => 'textfield',
				'#title' => 'Final Section Alternate Name',
				'#description' => 'Normally Phase 3, but can be changed.',
				'#default_value' => variable_get('cl_final_section_name'),
				'#maxlength' => 128,
				'#required' => TRUE,
			),
			'phase_3_description' => array(
				'#type' => 'textarea',
				'#title' => variable_get('cl_final_section_name').' Help Text.',
				'#description' => 'Help text for users in the final section.',
				'#default_value' => variable_get('cl_phase_3_description'),
				'#required' => TRUE,
			),
			'application_description' => array(
				'#type' => 'textarea',
				'#title' => 'Application Help Text.',
				'#description' => 'Help text for users in the application section.',
				'#default_value' => variable_get('cl_application_description'),
				'#required' => TRUE,
			),
		),
		'submit' => array(
			'#type' => 'submit',
			'#value' => 'Save Settings',
		),
	);
}

function cl_admin_settings_form_validate($form, &$form_state) {

}

function cl_admin_settings_form_submit($form, &$form_state) {
	//dpm($form_state);
	variable_set('cl_program_name', $form_state['input']['program_name']);
	variable_set('cl_lock_status', $form_state['input']['lock_status']);
	variable_set('cl_redirect_target', $form_state['input']['redirect_target']);
	variable_set('cl_default_app_reject', $form_state['input']['default_app_reject']);
	variable_set('cl_default_app_accept', $form_state['input']['default_app_accept']);
	variable_set('cl_final_section_name', $form_state['input']['final_section_name']);
	variable_set('cl_phase_3_description', $form_state['input']['phase_3_description']);
	variable_set('cl_application_description', $form_state['input']['application_description']);
	
	drupal_flush_all_caches();
	drupal_set_message('Settings saved. All caches cleared.');
	//variable_set('cl_', $form_state['input']['']);
	//variable_set('cl_', $form_state['input']['']);
	//variable_set('cl_', $form_state['input']['']);
	//variable_set('cl_', $form_state['input']['']);
}
?>