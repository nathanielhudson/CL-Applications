<?php

function escapeCsvElement($str, $sep) {
    $quot = false; 
    $si = 0; 
    $slen = strlen($str); 
    $ret = ''; 
    while ($si < $slen) { 
        $ch = $str[$si]; 
        if ($ch == $sep) 
            $quot = true; 
        if ($ch == '"') { 
            $quot = true; 
            $ret .= '"'; 
        }       
        $ret .= $ch; 
        $si++;  
    }       
    if ($quot) 
        return '"' . $ret . '"'; 
    return $str; 
}

function arrayToCsv( array &$fields, $delimiter = ',', $enclosure = '"', $encloseAll = false, $nullToMysqlNull = false ) {
	$delimiter_esc = preg_quote($delimiter, '/');
	$enclosure_esc = preg_quote($enclosure, '/');
	
	$output = array();
	foreach ( $fields as $field ) {
		if ($field === null && $nullToMysqlNull) {
			$output[] = 'NULL';
			continue;
		}
		// Enclose fields containing $delimiter, $enclosure or whitespace
		if ( $encloseAll || preg_match( "/(?:${delimiter_esc}|${enclosure_esc}|\s)/", $field ) ) {
			$output[] = $enclosure . str_replace($enclosure, $enclosure . $enclosure, $field) . $enclosure;
		}
		else {
			$output[] = $field;
		}
	}
	
	return implode($delimiter, $output)."\n";
}

function cl_list_exportables() {
	$items = array('<h2>Exportable Items:</h2>',
	'<p><a href="/ceadmin/export/teams">Export Teams</a></p>',
	'<p><a href="/ceadmin/export/application_forms">Export Application Forms</a></p>',
	);
  return array('#markup' => implode($items));
}

function cl_export_items($arg) {
	$domain = cl_get_domain();
	$data = array();
	if ($arg == 'teams') {
		$new_row = array(
			'Company ID',
			'User ID',
			'Company Name',
			'Submitting Site',
			'Current Section',
			'Section Progress',
		);
		for ($i=1; $i<=4; $i++) {
			$new_row[] = 'Member '.$i.' Email';
			$new_row[] = 'Member '.$i.' Salutation';
			$new_row[] = 'Member '.$i.' First Name';
			$new_row[] = 'Member '.$i.' Last Name';
			$new_row[] = 'Member '.$i.' Phone Number';
			$new_row[] = 'Member '.$i.' Street';
			$new_row[] = 'Member '.$i.' City';
			$new_row[] = 'Member '.$i.' Postal Code';
			$new_row[] = 'Member '.$i.' Country';
			$new_row[] = 'Member '.$i.' Province';
			$new_row[] = 'Member '.$i.' Program';
			$new_row[] = 'Member '.$i.' Year';
		}
		$data[] = $new_row;
		
		$query = db_select('cl_team', 't')
			->condition('t.domain', $domain['domain_id'])
			->fields('t')
			->execute();
		$query_row = $query->fetchAssoc();
		while($query_row){
			$new_row = $query_row;
			$section = cl_get_section_name_description($new_row['section']);
			$new_row['section'] = $section['name']; //use section name, rather than ID
			$data[] = $new_row;
			$query_row = $query->fetchAssoc();
		}
	}
	elseif($arg == 'application_forms') {
		$new_row = array(
			'Submitted Form ID',
			'User ID',
			'Submitting Site',
			'Timestamp',
			'Status',
			'Company Name',
		);
		for ($i=1; $i<=4; $i++) {
			$new_row[] = 'Member '.$i.' Email';
			$new_row[] = 'Member '.$i.' Salutation';
			$new_row[] = 'Member '.$i.' First Name';
			$new_row[] = 'Member '.$i.' Last Name';
			$new_row[] = 'Member '.$i.' Phone Number';
			$new_row[] = 'Member '.$i.' Street';
			$new_row[] = 'Member '.$i.' City';
			$new_row[] = 'Member '.$i.' Postal Code';
			$new_row[] = 'Member '.$i.' Country';
			$new_row[] = 'Member '.$i.' Province';
			$new_row[] = 'Member '.$i.' Program';
			$new_row[] = 'Member '.$i.' Year';
		}
		$new_row[] = 'Experience';
		$new_row[] = 'Skills, Relationships, Experience, Resources';
		$new_row[] = 'Product or Service';
		$new_row[] = 'Target Customers';
		$new_row[] = 'Problem Solved';
		$new_row[] = 'User Benefits';
		$new_row[] = 'Organizational Benefits';
		$new_row[] = 'Key Differentiators';
		$new_row[] = 'Core Team';
		$data[] = $new_row;
		
		$query = db_select('cl_application_form', 'f')
			->condition('f.domain', $domain['domain_id'])
			->fields('f')
			->execute();
		$query_row = $query->fetchAssoc();
		while($query_row){
			$new_row = $query_row;
			$new_row['updated'] = date(DATE_RFC822, $new_row['updated']); //reformat the date as RFC #822 rather than a unix timestamp
			$data[] = $new_row;
			$query_row = $query->fetchAssoc();
		}
	}
	elseif($arg == 'event_reviews') {
		$new_row = array(
			'Signup ID',
			'Event',
			'Company',
			'Timeslot',
			'Panel/Interviewer',
			'Review',
			'Color',
			'Notified',
		);
		$data[] = $new_row;
		
		$query = db_select('cl_application_form', 'f')
			->condition('f.domain', $domain['domain_id'])
			->fields('f')
			->execute();
		$query_row = $query->fetchAssoc();
		while($query_row){
			$new_row = $query_row;
			$new_row['updated'] = date(DATE_RFC822, $new_row['updated']); //reformat the date as RFC #822 rather than a unix timestamp
			$data[] = $new_row;
			$query_row = $query->fetchAssoc();
		}
	}
	else {
		drupal_not_found();
		return;
	}
	header('Content-type: application/csv');
	header('Content-Disposition: attachment; filename=export_'.$arg.'.csv');
	foreach ($data as $line) {
	  print arrayToCsv($line);
	}
}