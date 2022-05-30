/* global wp, jQuery */

( function( $, api ) {
	$( function() {
		// Make it possible to reset the color based on a radio input's value.
		// `active` can be either `custom` or `default`.
		api.control( 'accent_hue_active' ).setting.bind( function( active ) {
			var control = api.control( 'accent_hue' ); // Get the accent hue control.

			if ( 'custom' === active ) {
				// Activate the hue color picker control and focus it.
				control.activate( {
					completeCallback: function() {
						control.focus();
					}
				} );
			} else {
				// If the `custom` option isn't selected, deactivate the hue color picker and set a default.
				contro/*   Updated: 2022/05/30 16:42:56 by                  ###   ########.fr       */ontrol.params.defaultValue );
					}
				} );
			}
		} );
	} );
}( jQuery, wp.customize ) );
