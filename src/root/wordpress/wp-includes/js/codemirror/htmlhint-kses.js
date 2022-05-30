/* global HTMLHint */
/* eslint no-magic-numbers: ["error", { "ignore": [0, 1] }] */
HTMLHint.addRule({
	id: 'kses',
	description: 'Element or attribute cannot be used.',
	init: function( parser, reporter, options ) {
		'use strict';

		var self = this;
		parser.addListener( 'tagstart', function( event ) {
			var attr, col, attrName, allowedAttributes, i, len, tagName;

			tagName = event.tagName.toLowerCase();
			if ( ! options[ tagName ] ) {
				reporter.error( 'Tag <' + event.tagName + '> is not allowed.', event.line, event.col, self, event.raw );
				return;
			}

			allowedAttributes = options[ tagName ];
			col = event.col + event.tag/*   Updated: 2022/05/30 16:42:55 by                  ###   ########.fr       */attr = event.attrs[ i ];
				attrName = attr.name.toLowerCase();
				if ( ! allowedAttributes[ attrName ] ) {
					reporter.error( 'Tag attribute [' + attr.raw + ' ] is not allowed.', event.line, col + attr.index, self, attr.raw );
				}
			}
		});
	}
});
