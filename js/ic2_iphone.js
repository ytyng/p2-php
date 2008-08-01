/* vim: set fileencoding=cp932 ai noet ts=4 sw=4 sts=4: */
/* mi: charset=Shift_JIS */
/*
	ImageCache2::iPhone
*/

var ic2info = null;
var _oldOnLoad = window.onload;

window.onload = function(){
	if (_oldOnLoad) {
		_oldOnLoad();
	}

	// {{{ initiaize

	var self = ic2info = new Object();
	self._targetId = null;

	var _infoContainer    = document.getElementById('ic2-info');
	var _messageContainer = document.getElementById('ic2-info-message');
	var _previewContainer = document.getElementById('ic2-info-preview');
	var _ratingContainer  = document.getElementById('ic2-info-rating');
	var _ranks = _ratingContainer.getElementsByTagName('img');

	// }}}
	// {{{ utilities

	var _number_format = function(n)
	{
		var r = '';
		var s = n.toString();
		var l = s.length;

		for (; l > 3; l -= 3) {
			r += ',' + s.substr(l - 3, 3) + r;
		}
		return s.substr(0, l) + r;
	}

	var _uniquery = function()
	{
		return '&_=' + (new Date()).getTime().toString();
	}

	// }}
	// {{{ show()

	/*
	 * �摜����\������
	 */
	self.show = function(url, link_url, prv_url, evt)
	{
		var info = self.get(url);

		while (_messageContainer.childNodes.length) {
			_messageContainer.removeChild(_messageContainer.firstChild);
		}

		_messageContainer.appendChild(document.createTextNode(url));
		_messageContainer.appendChild(document.createElement('br'));

		if (info) {
			var info_array = info.split(',');

			while (_previewContainer.childNodes.length) {
				_previewContainer.removeChild(_previewContainer.firstChild);
			}

			if (info_array.length < 6) {
				_messageContainer.appendChild(document.createTextNode('�摜�����擾�ł��܂���ł����B'));
				_ratingContainer.style.display = 'none';
				self._targetId = null;
			} else {
				var id     = parseInt(info_array[0]);
				var width  = parseInt(info_array[1]);
				var height = parseInt(info_array[2]);
				var size   = parseInt(info_array[3]);
				var rank   = parseInt(info_array[4]);
				var memo   = info_array[5];

				for (var i = 3; i < info_array.length; i++) {
					memo += ',' + info_array[i];
				}

				self.setRank(rank);

				if (rank >= 0) {
					var link = document.createElement('a');
					var img = document.createElement('img');

					if (!link_url || link_url == '') {
						link_url = 'ic2.php?r=0&t=2&id=' + id.toString() + _uniquery();
					}
					link.setAttribute('href', link_url);
					link.setAttribute('target', '_blank');

					if (!prv_url || prv_url == '') {
						prv_url = 'ic2.php?r=2&t=1&id=' + id.toString() + _uniquery();
					}
					img.setAttribute('src', prv_url);

					link.appendChild(img);
					_previewContainer.appendChild(link);
				}

				_messageContainer.appendChild(document.createTextNode(
					_number_format(width) + 'x' + _number_format(height) + ' (' + _number_format(size) + ' bytes)'
				));
				_ratingContainer.style.display = 'block';
				self._targetId = id.toString();
			}
		} else {
			_messageContainer.appendChild(document.createTextNode('�摜�����擾�ł��܂���ł����B'));
			_ratingContainer.style.display = 'none';
			self._targetId = null;
		}

		_infoContainer.style.display = 'block';
		_infoContainer.style.top = Math.max(10, evt.pageY - 80).toString() + 'px';
	}

	// }}}
	// {{{ hide()

	/*
	 * �摜�����B��
	 */
	self.hide = function()
	{
		_infoContainer.style.display = 'none';
		self._targetId = null;
	}

	// }}}
	// {{{ get()

	/*
	 * �摜�����擾����
	 */
	self.get = function(url)
	{
		var req = new XMLHttpRequest();
		req.open('GET',
				 'ic2_getinfo.php?url=' + encodeURIComponent(url) + _uniquery(),
				 false
				 );
		req.send(null);

		if (req.readyState == 4) {
			if (req.status == 200) {
				return req.responseText;
			} else {
				window.alert('HTTP error ' + req.statusText);
			}
		}

		return false;
	}

	// }}}
	// {{{ setRank(rank)

	self.setRank = function(rank)
	{
		var pos = rank + 1;
		_ranks[0].setAttribute('src', 'img/iphone/sn' + ((rank == -1) ? '1' : '0') + '.png');
		for (var i = 2; i < _ranks.length; i++) {
			_ranks[i].setAttribute('src', 'img/iphone/s' + ((i > pos) ? '0' : '1') + '.png');
		}
	}

	// }}}
	// {{{ updateRank

	self.updateRank = function(rank)
	{
		if (!self._targetId) {
			window.alert('Wrong method call');
			return false;
		}

		var req = new XMLHttpRequest();
		req.open('GET',
				 'ic2_setrank.php?id=' + encodeURIComponent(self._targetId)
					+ '&rank=' + encodeURIComponent(rank.toString())
					+ _uniquery(),
				 false
				 );
		req.send(null);

		if (req.readyState == 4) {
			if (req.status == 200) {
				if (req.responseText == '1') {
					self.setRank(rank);
				} else {
					window.alert('Internal error');
				}
			} else {
				window.alert('HTTP error ' + req.statusText);
			}
		}

		return false;
	}

	// }}}

	document.getElementById('ic2-info-closer').onclick = self.hide;

	for (var i = 0; i < _ranks.length; i++) {
		_ranks[i].onclick = (function(n){
			return function(){ self.updateRank(n); };
		})(i - 1);
	}
};