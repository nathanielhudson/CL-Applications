(function ($) {
Drupal.behaviors.cl_event_table_print = {
	attach: function (context, settings) {
		
		function encodeID(s) {
		    if (s==='') return '_';
		    return s.replace(/[^a-zA-Z0-9.-]/g, function(match) {
		        return '_'+match[0].charCodeAt(0).toString(16)+'_';
		    });
		}
		
		function encodeHTML(s) {
		    return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/"/g, '&quot;');
		}
		
		function pausecomp(ms) {
			ms += new Date().getTime();
			while (new Date() < ms){}
		}
		
		
		$("#content table").after('<hr class="hidden-hr"/>');
		
		
		$("#content table").after('<div id="cl_print_basic_control" class="clearfix print_button"><input type="checkbox" id="cl_print_basic" /><label for="cl_print_basic">Print</label></div>');
		$("#cl_print_basic").button();
		
		//$("#content table").after('<div id="cl_print_lines_control"><input type="checkbox" id="cl_print_lines" /><label for="cl_print_lines">Print With Lines</label></div>');
		//$("#cl_print_lines").button();
		
		
		$("#cl_print_basic").change(function () {
			window.print();
		});
		
		var column=1; //ensure no id collisions
		$("#content thead th").each(function () {
			if ($(this).text() != "Time") {
				var id = encodeID("cl_print_"+$(this).text()+column);
				column += 1;
				
				$("#content table").after('<div id="'+id+'_control" class="clearfix print_button"><input type="checkbox" id="'+id+'" /><label for="'+id+'">Print Notes Page for '+encodeHTML($(this).text())+'</label></div>');
				$("#"+id).button();
				$("#"+id).data('target_col', column);
				
				$("#"+id).change(function () {
					var line = '<hr class="printline" />';
					var target_col = $(this).data('target_col');
					$(".cl_event_item a").after(line).after(line).after(line).after(line);
					$('td:not(:nth-child('+target_col+'),:nth-child(1))').hide();
					$('th:not(:nth-child('+target_col+'),:nth-child(1))').hide();
					pausecomp(500); //for whatever reason, sometimes the dom takes a bit to update in webkit...
					window.print();
					$('td:not(:nth-child('+target_col+'),:nth-child(1))').show();
					$('th:not(:nth-child('+target_col+'),:nth-child(1))').show();
					$('.printline').remove();
					$(this).attr('checked', true);
				});
				
			}
		});
		
		/*$("#cl_print_lines").change(function () {
			var line = '<hr class="printline" />';
			$("#content .cl_event_item").after(line).after(line).after(line).after(line);
			window.print();
		});*/
		
		 
	}
};
}(jQuery));

