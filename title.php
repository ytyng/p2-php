<?php
// p2 -  �^�C�g���y�[�W

include("./conf.php");   //��{�ݒ�t�@�C���Ǎ�
require_once './p2util.class.php';	// p2�p�̃��[�e�B���e�B�N���X
require_once("./filectl_class.inc");
require_once("./datactl.inc");

authorize(); //���[�U�F��

//=========================================================
// �ϐ�
//=========================================================
$_info_msg_ht = "";

/*
if ($through_ime=="2ch") {
	$url = parse_url($p2web_url);
	$p2web_url_r = $url[scheme] . "://ime.nu/" . $url[host] . $url[path];
} else {
	$p2web_url_r = $p2web_url;
}
*/

$p2web_url_r = throughIme($p2web_url);

//�p�[�~�b�V�������ӊ��N================
if ($prefdir==$datdir) {
	datadir_writable_check($prefdir);
} else {
	datadir_writable_check($prefdir);
	datadir_writable_check($datdir);
}

//=========================================================
// ��ID 2ch �I�[�g���O�C��
//=========================================================
if(file_exists($idpw2ch_php)){
	include($idpw2ch_php);
	$login2chPW = base64_decode($login2chPW);
	include_once("./crypt_xor.inc");
	$login2chPW = decrypt_xor($login2chPW, $crypt_xor_key);
	if($autoLogin2ch){
		include_once("./login2ch.inc");
		login2ch();
	}
}


// �ŐV�Ń`�F�b�N
if ($updatan_haahaa) {
	$newversion_found = checkUpdatan();
}

// �F�؃��[�U���
$autho_user_ht = "";
if ($login['use']) {
	$autho_user_ht = "<p>���O�C�����[�U: {$login['user']} - ".date("Y/m/d (D) G:i")."</p>\n";
}

// �O��̃��O�C�����
$last_login_ht = "";
if ($_conf['login_log_rec'] && $_conf['last_login_log_show']) {
	if (($alog = P2Util::getLastAccessLog($_conf['login_log_file'])) !== false) {
		$last_login_ht =<<<EOP
�O��̃��O�C����� - {$alog['date']}<br>
���[�U: {$alog['user']}<br>
IP: {$alog['ip']}<br>
HOST: {$alog['host']}<br>
UA: {$alog['ua']}<br>
REFERER: {$alog['referer']}
EOP;
	}
/*
	$last_login_ht =<<<EOP
<table cellspacing="0" cellpadding="2";>
	<tr>
		<td colspan="2">�O��̃��O�C�����</td>
	</tr>
	<tr>
		<td align="right">����: </td><td>{$alog['date']}</td>
	</tr>
	<tr>
		<td align="right">���[�U: </td><td>{$alog['user']}</td>
	</tr>
	<tr>
		<td align="right">IP: </td><td>{$alog['ip']}</td>
	</tr>
	<tr>
		<td align="right">HOST: </td><td>{$alog['host']}</td>
	</tr>
	<tr>
		<td align="right">UA: </td><td>{$alog['ua']}</td>
	</tr>
	<tr>
		<td align="right">REFERER: </td><td>{$alog['referer']}</td>
</table>
EOP;
*/
} else {
	$last_login_ht = "";
}

//=========================================================
// HTML�v�����g
//=========================================================
$ptitle = "p2 - title";

header_content_type();
if($doctype){ echo $doctype;}
echo <<<EOP
<html lang="ja">
<head>
	<meta name="ROBOTS" content="NOINDEX, NOFOLLOW">
	<meta http-equiv="Content-Style-Type" content="text/css">
	<meta http-equiv="Content-Script-Type" content="text/javascript">
	<title>{$ptitle}</title>
	<base target="read">
EOP;

@include("./style/style_css.inc");

echo <<<EOP
</head>
<body>
EOP;

echo $_info_msg_ht;
$_info_msg_ht="";

echo <<<EOP
<br>
<div class="container">
	{$newversion_found}
	<p>p2 version {$p2version} �@<a href="{$p2web_url_r}" target="_blank">{$p2web_url}</a></p>
	<ul>
		<li><a href="viewtxt.php?file=doc/README.txt">README.txt</a></li>
		<li><a href="img/how_to_use.png">�����ȒP�ȑ���@</a></li>
		<li><a href="viewtxt.php?file=doc/ChangeLog.txt">ChangeLog�i�X�V�L�^�j</a></li>
	</ul>
	<!-- <p><a href="{$p2web_url_r}" target="_blank">p2 web &lt;{$p2web_url}&gt;</a></p> -->
	{$autho_user_ht}
	{$last_login_ht}
</div>
</body>
</html>
EOP;

//==================================================
// ���֐�
//==================================================
/**
* �I�����C�����p2�ŐV�ł��`�F�b�N����֐�
*/
function checkUpdatan(){
	global $p2web_url, $p2web_url_r, $prefdir, $p2version;

	$ver_txt_url = $p2web_url . "p2status.txt";
	$cachefile = $prefdir . "/p2_cache/p2status.txt";
	FileCtl::mkdir_for($cachefile);
	fileDownload($ver_txt_url, $cachefile);
	$ver_txt=file($cachefile);
	$update_ver=$ver_txt[0];
	$kita="�����������i߁�߁j��������!!!!!!";
	//$kita="��*��ߥ*:.�..�.:*��(߁��)ߥ*:.�. .�.:*��ߥ*!!!!!";
	if( preg_match("/^\d\.\d\.\d$/", $update_ver) and ($update_ver > $p2version) ){
		$newversion_found =<<<EOP
<div class="kakomi">
	{$kita}<br>
	�I�����C����� p2 �̍ŐV�o�[�W�����������܂����B<br>
	p2 version {$update_ver} �� <a href="{$p2web_url_r}cgi/dl/dl.php?dl=p2">�_�E�����[�h</a> / <a href="{$p2web_url_r}p2/doc/ChangeLog.txt"{$ext_win_target}>�X�V�L�^</a>
</div>
<hr class="invisible">
EOP;
	}
	return $newversion_found;
}

?>