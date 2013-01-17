(function ($) {
Drupal.behaviors.cl_history = {
	attach: function (context, settings) {
		if (Drupal.settings.cl_application.target) {
  			$('#history-'+Drupal.settings.cl_application.target).addClass("target");
			$.scrollTo('#history-'+Drupal.settings.cl_application.target, 1000, {offset:-50, duration:3000} );
		}
	}
};
}(jQuery));