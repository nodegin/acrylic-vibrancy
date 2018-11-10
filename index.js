var Vibrancy = require('bindings')('Vibrancy');

module.exports = {
	SetAcrylic(window) {
		return Vibrancy.SetAcrylic(window.getNativeWindowHandle());
	},
}
