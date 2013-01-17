(function ($) {
Drupal.behaviors.cl_history = {
	attach: function (context, settings) {
		
		/*******************************************************
		 ***      STUFF TO MOVE TEAMS AROUND THE TABLE       ***
		 *******************************************************/
		 		
		function getPanelQuery(item) {
			var cl_event_panels = new Array();
			$("#cl_event_admin_table th").each(function (index) {
				cl_event_panels[$(this).text()] = (index + 1);
				//console.log("INDEX "+index+"  TEXT "+$(this).text());
			})
			var val = item.parent().parent().find("select.panel_selector").val();
			if (typeof (cl_event_panels[val]) != 'undefined') {
				return "td:nth-child(" + cl_event_panels[val] + ")";
			} else {
				return "td:last-child";
			}
		}
		
		function getSlotQuery(item) {
			var cl_event_slots = new Array();
			$("#cl_event_admin_table tr td:first-child").each(function (index) {
				cl_event_slots[$(this).text()] = (index + 1);
				//console.log("INDEX "+index+"  TEXT "+$(this).text());
			})
			var val = item.parent().parent().find("select.slot_selector").val();
			if (typeof (cl_event_slots[val]) != 'undefined') {
				return "tr:nth-child(" + cl_event_slots[val] + ")";
			} else {
				return "tr:last-child";
			}
		}
	
		$("#cl_event_admin_table select").change(function () {
        	$(this).parent().parent().parent()
        	.appendTo('#cl_event_admin_table tbody '+getSlotQuery($(this))+' '+getPanelQuery($(this)))
        	.animate({backgroundColor: "#fffa61"}, 100).animate({backgroundColor: "#ccc"}, 1000);
        	
        	/* Started writing js validation. Wasn't really worth finishing.
        	if($(this).parent().parent().parent().parent().find("select.panel_selector").length > 1) {
        		$(this).parent().parent().parent().find().animate({backgroundColor: "#ecacac"}, 1000);
        	} else {
        		$(this).parent().parent().parent().animate({backgroundColor: "#ccc"}, 1000);
        	}*/
    	});
    	
		$("#cl_event_admin_table select.panel_selector").change();
		$('<div class="tabledrag-changed-warning messages warning">Changes made in this table will not be saved until the form is submitted.</div>').insertBefore("#cl_event_admin_table").hide().fadeIn('slow');
		
		/*******************************************************
		 ***   INTERVIEW TIME LOCKING AND UNLOCKING STUFF    ***
		 *******************************************************/

		var warned = false;

		if (Drupal.settings.cl_application.event_type == "interview") {
			$("#cl_event_admin_table select.slot_selector").hide();
			$("#cl_event_admin_table").after('<div id="cl_event_show_times_control"><input type="checkbox" id="cl_event_show_times" /><label for="cl_event_show_times">Unlock Times</label></div>');
			$("#cl_event_show_times").button();
		}
		
		$("#cl_event_show_times").change(function () {
			if (!warned) {
				$("#cl_event_admin_table").after('<div id="cl_show_times_warn" title="Are You Sure?"><p>Are you sure you want to edit times? <em>These are times that users have selected</em> on the signup page for this event. Please exercise caution, and check with users before changing any times.</p></div>');
				$("#cl_show_times_warn").dialog({
            		height: 240,
            		modal: true,
            		buttons: {
                		"Edit Anyways": function() {
                    		warned = true;
                    		$("#cl_event_admin_table select.slot_selector").toggle();
                    		$( this ).dialog( "close" );
                		},
                		Cancel: function() {
                    		$( this ).dialog( "close" );
                		}
            		}
        		});
			} else {
				$("#cl_event_admin_table select.slot_selector").toggle();
			}
    	});
    	
    	/*******************************************************
		 ***                 AUTOFILL STUFF                  ***
		 *******************************************************/
		 
		/*
		$('<button id="auto_times" class="event_button" type="button">Autofill Times</button>').insertAfter("#cl_event_admin_table").button().click(function () {
			$(this).hide();
			var cl_event_slots = new Array();
			var cl_event_panels = new Array();
			$("#cl_event_admin_table tr td:first-child").each(function (index) {
				cl_event_slots.push($(this).text());
			})
			$("#cl_event_admin_table th").each(function (index) {
				cl_event_panels.push($(this).text());
			})
			var slot_counter = 0;
			var panel_counter = 0;
			$("#cl_event_admin_table .cl_event_item .slot_selector").each(function (index) {
				$(this).val(cl_event_slots[slot_counter]).change();
				slot_counter++;
			})
		});*/
		 
	}
};
}(jQuery));

