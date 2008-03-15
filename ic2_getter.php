<?php
/* vim: set fileencoding=cp932 ai et ts=4 sw=4 sts=4 fdm=marker: */
/* mi: charset=Shift_JIS */

/* ImageCache2 - �_�E�����[�_ */

// {{{ p2��{�ݒ�ǂݍ���&�F��

require_once 'conf/conf.inc.php';

$_login->authorize();

if (!$_conf['expack.ic2.enabled']) {
    exit('<html><body><p>ImageCache2�͖����ł��B<br>conf/conf_admin_ex.inc.php �̐ݒ��ς��Ă��������B</p></body></html>');
}

// }}}
// {{{ ������

// conf.inc.php�ňꊇstripslashes()���Ă��邯�ǁAHTML_QuickForm�ł��Ǝ���stripslashes()����̂ŁB
// �o�O�̉����ƂȂ�\�����ے�ł��Ȃ��E�E�E
if (get_magic_quotes_gpc()) {
    $_GET = array_map('addslashes_r', $_GET);
    $_POST = array_map('addslashes_r', $_POST);
    $_REQUEST = array_map('addslashes_r', $_REQUEST);
}

// ���C�u�����ǂݍ���
require_once 'HTML/QuickForm.php';
require_once 'HTML/QuickForm/Renderer/ObjectFlexy.php';
require_once 'HTML/Template/Flexy.php';
require_once P2EX_LIBRARY_DIR . '/ic2/loadconfig.inc.php';
require_once P2EX_LIBRARY_DIR . '/ic2/database.class.php';
require_once P2EX_LIBRARY_DIR . '/ic2/db_images.class.php';
require_once P2EX_LIBRARY_DIR . '/ic2/thumbnail.class.php';

// �|�b�v�A�b�v�E�C���h�E�H
$isPopUp = empty($_GET['popup']) ? 0 : 1;


// }}}
// {{{ config


// �ݒ�t�@�C���ǂݍ���
$ini = ic2_loadconfig();

// DB_DataObject�̐ݒ�
$_dbdo_options = &PEAR::getStaticProperty('DB_DataObject','options');
$_dbdo_options = array('database' => $ini['General']['dsn'], 'debug' => FALSE, 'quote_identifiers' => TRUE);

// �t�H�[���̃f�t�H���g�l
$qf_defaults = array(
    'uri'  => 'http://',
    'memo'    => '',
    'from'    => 'from',
    'to'      => 'to',
    'padding' => '',
    'popup'   => $isPopUp,
);

// �t�H�[���̌Œ�l
$qf_constants = array(
    'detect_hint' => '�����@����',
    'download'    => '�_�E�����[�h',
    'reset'       => '���Z�b�g',
    'close'       => '����',
);

// �v���r���[�̑傫��
$_preview_size = array(
    '1' => $ini['Thumb1']['width'] . '&times;' . $ini['Thumb1']['height'],
    '2' => $ini['Thumb2']['width'] . '&times;' . $ini['Thumb2']['height'],
    '3' => $ini['Thumb3']['width'] . '&times;' . $ini['Thumb3']['height'],
);

// ����
$_attr_uri    = array('size' => 50, 'onchange' => 'checkSerial(this.value)');
$_attr_s_chk  = array('onclick' => 'setSerialAvailable(this.checked)', 'id' => 's_chk');
$_attr_s_from = array('size' => 4, 'id' => 's_from');
$_attr_s_to   = array('size' => 4, 'id' => 's_to');
$_attr_s_pad  = array('size' => 1, 'id' => 's_pad');
$_attr_memo   = array('size' => 50);
$_attr_submit = array();
$_attr_reset  = array();
$_attr_close  = array('onclick' => 'window.close()');


// }}}
// {{{ prepare (Form & Template)


// �摜�_�E�����[�h�p�t�H�[����ݒ�
$_attribures = array('accept-charset' => 'UTF-8,Shift_JIS');
$_target = $isPopUp ? '_self' : 'read';

$qf = &new HTML_QuickForm('get', 'get', $_SERVER['SCRIPT_NAME'], $_target, $_attribures);
$qf->setDefaults($qf_defaults);
$qf->setConstants($qf_constants);

// �t�H�[���v�f�̒�`
$qfe = array();

// �B���v�f
$qfe['detect_hint'] = &$qf->addElement('hidden', 'detect_hint');
$qfe['popup'] = &$qf->addElement('hidden', 'popup');

// URL�ƘA�Ԑݒ�
$qfe['uri']     = &$qf->addElement('text', 'uri', 'URL', $_attr_uri);
$qfe['serial']  = &$qf->addElement('checkbox', 'serial', '�A��', NULL, $_attr_s_chk);
$qfe['from']    = &$qf->addElement('text', 'from', 'From', $_attr_s_from);
$qfe['to']      = &$qf->addElement('text', 'to', 'To', $_attr_s_to);
$qfe['padding'] = &$qf->addElement('text', 'padding', '0�ŋl�߂錅��', $_attr_s_pad);

// ����
$qfe['memo'] = &$qf->addElement('text', 'memo', '����', $_attr_memo);

// �v���r���[�̑傫��
$preview_size = array();
foreach ($_preview_size as $value => $lavel) {
    $preview_size[$value] = &HTML_QuickForm::createElement('radio', NULL, NULL, $lavel, $value);
}
$qf->addGroup($preview_size, 'preview_size', '�v���r���[', '&nbsp;');
if (!isset($_GET['preview_size'])) {
    $preview_size[1]->updateAttributes('checked="checked"');
}

// ����E���Z�b�g�E����
$qfe['download'] = &$qf->addElement('submit', 'download');
$qfe['reset']    = &$qf->addElement('reset', 'reset');
$qfe['close']    = &$qf->addElement('button', 'close', NULL, $_attr_close);

// Flexy
$_flexy_options = array(
    'locale' => 'ja',
    'charset' => 'cp932',
    'compileDir' => $ini['General']['cachedir'] . '/' . $ini['General']['compiledir'],
    'templateDir' => P2EX_LIBRARY_DIR . '/ic2/templates',
    'numberFormat' => '', // ",0,'.',','" �Ɠ���
);

$flexy = &new HTML_Template_Flexy($_flexy_options);

$flexy->setData('php_self', $_SERVER['SCRIPT_NAME']);
$flexy->setData('skin', $skin_en);
$flexy->setData('isPopUp', $isPopUp);


// }}}
// {{{ validate

$execDL = FALSE;
if ($qf->validate() && ($params = $qf->getSubmitValues()) && isset($params['uri']) && isset($params['download'])) {
    $execDL = TRUE;
    $params = array_map('trim', $params);

    // URL������
    $purl = @parse_url($params['uri']);
    if (!$purl || !preg_match('/^(https?)$/', $purl['scheme']) || empty($purl['host']) || empty($purl['path'])) {
        $_info_msg_ht .= '<p>�G���[: �s����URL</p>';
        $execDL = FALSE;
        $isError = TRUE;
    }

    // �v���r���[�̑傫��
    if (isset($params['preview_size']) && in_array($params['preview_size'], array_keys($_preview_size))) {
        $thumb_type = (int)$params['preview_size'];
    } else {
        $thumb_type = 1;
    }

    // ����
    if (isset($params['memo']) && strlen(trim($params['memo'])) > 0) {
        $new_memo = IC2DB_Images::uniform($params['memo'], 'SJIS-win');
        $_memo_en = rawurlencode($new_memo);
        $_hint_en = rawurlencode(mb_convert_encoding('�����@����', 'UTF-8', 'SJIS-win'));
        // �����_�����O����htmlspecialchars()�����̂ŁA�����ł�&��&amp;�ɂ��Ȃ�
        $append_memo = '&detect_hint=' . $_hint_en . '&memo=' . $_memo_en;
    } else {
        $new_memo = NULL;
        $append_memo = '';
    }

    // �A��
    $serial_pattern = '/\\[(\\d+)-(\\d+)\\]/';
    if (!empty($params['serial'])) {

        // �v���[�X�z���_�ƃ��[�U�w��p�����[�^
        if (strstr($params['uri'], '%s') && !preg_match($serial_pattern, $params['uri'], $from_to)) {
            if (strstr(preg_replace('/%s/', ' ', $params['uri'], 1), '%s')) {
                $_info_msg_ht .= '<p>�G���[: URL�Ɋ܂߂���v���[�X�z���_�͈�����ł��B</p>';
                $execDL = FALSE;
                $isError = TRUE;
            } elseif (preg_match('/\\D/', $params['from']) || strlen($params['from']) == 0 ||
                      preg_match('/\\D/', $params['to'])   || strlen($params['to'])   == 0 ||
                      preg_match('/\\D/', $params['padding'])
            ) {
                $_info_msg_ht .= '<p>�G���[: �A�ԃp�����[�^�Ɍ�肪����܂��B</p>';
                $execDL = FALSE;
                $isError = TRUE;
            } else {
                $serial = array();
                $serial['from'] = (int)$params['from'];
                $serial['to']   = (int)$params['to'];
                if (strlen($params['padding']) == 0) {
                    $serial['pad'] = strlen($serial['to']);
                } else {
                    $serial['pad']  = (int)$params['padding'];
                }
             }

        // [from-to] ��W�J
        } elseif (preg_match($serial_pattern, $params['uri'], $from_to) && !strstr($params['uri'], '%s')) {
            $params['uri'] = preg_replace($serial_pattern, '%s', $params['uri'], 1);
            if (preg_match($serial_pattern, $params['uri'])) {
                $_info_msg_ht .= '<p>�G���[: URL�Ɋ܂߂���A�ԃp�^�[���͈�����ł��B</p>';
                $execDL = FALSE;
                $isError = TRUE;
            } else {
                $serial = array();
                $serial['from'] = (int)$from_to[1];
                $serial['to']   = (int)$from_to[2];
                if (strlen($from_to[1]) == strlen($from_to[2])) {
                    $serial['pad'] = strlen($from_to[2]);
                /*} elseif (strlen($from_to[1]) < strlen($from_to[2]) && strlen($from_to[1]) > 1 && substr($from_to[1]) == '0') {
                    $serial['pad'] = strlen($from_to[1]);*/
                } else {
                    $serial['pad'] = 0;
                }
            }

        // �ǂ�����������A����������
        } else {
            $_info_msg_ht .= '<p>�G���[: URL�ɘA�Ԃ̃v���[�X�z���_(<samp>%s</samp>)�܂��̓p�^�[��(<samp>[from-to]</samp>)���܂܂�Ă��Ȃ����A�������܂܂�Ă��܂��B</p>';
            $execDL = FALSE;
            $isError = TRUE;
        }

        // �͈͂�����
        if (isset($serial) && $serial['from'] >= $serial['to']) {
            $_info_msg_ht .= '<p>�G���[: �A�Ԃ̏I��̔ԍ��͎n�܂�̔ԍ����傫���Ȃ��Ƃ����܂���B</p>';
            $execDL = FALSE;
            $isError = TRUE;
            $serial = NULL;
        }

    // �A�ԂȂ�
    } else {
        if (strstr($params['uri'], '%s') || preg_match($serial_pattern, $params['uri'], $from_to)) {
            $_info_msg_ht .= '<p>�G���[: �A�ԂɃ`�F�b�N�������Ă��܂��񂪁AURL�ɘA�ԃ_�E�����[�h�p�̕����񂪊܂܂�Ă��܂��B</p>';
            $execDL = FALSE;
            $isError = TRUE;
        }
        $qfe['from']->updateAttributes('disabled="disabled"');
        $qfe['to']->updateAttributes('disabled="disabled"');
        $qfe['padding']->updateAttributes('disabled="disabled"');
        $serial = NULL;
    }

} else {
    $qfe['from']->updateAttributes('disabled="disabled"');
    $qfe['to']->updateAttributes('disabled="disabled"');
    $qfe['padding']->updateAttributes('disabled="disabled"');
}


// }}}
// {{{ generate


if ($execDL) {

    if (is_null($serial)) {
        $URLs = array($params['uri']);
    } else {
        $URLs = array();
        for ($i = $serial['from']; $i <= $serial['to']; $i++) {
            // URL�G���R�[�h���ꂽ�������%���܂ނ̂� sprintf() �͎g��Ȃ��B
            // URL�G���R�[�h�̃t�H�[�}�b�g��%+16�i���Ȃ̂�"%s"��u�����Ă��e�����Ȃ��B
            $URLs[] = str_replace('%s', str_pad($i, $serial['pad'], '0', STR_PAD_LEFT), $params['uri']);
        }
    }

    $thumbnailer = &new ThumbNailer($thumb_type);
    $images = array();

    foreach ($URLs as $url) {
        $icdb = &new IC2DB_Images;
        $img_title = htmlspecialchars($url, ENT_QUOTES);
        $url_en = rawurlencode($url);
        $src_url = 'ic2.php?r=1&uri=' . $url_en;
        $thumb_url = 'ic2.php?r=1&t=' . $thumb_type . '&uri=' . $url_en;
        $thumb_x = '';
        $thumb_y = '';
        $img_memo = $new_memo;

         // �摜���u���b�N���X�gor�G���[���O�ɂ���Ƃ�
        if (FALSE !== ($errcode = $icdb->ic2_isError($url))) {
            $img_title = "<s>{$img_title}</s>";
            $thumb_url = "./img/{$errcode}.png";

        // ���ɃL���b�V������Ă���Ƃ�
        } elseif ($icdb->get($url)) {
            $_src_url = $thumbnailer->srcPath($icdb->size, $icdb->md5, $icdb->mime);
            if (file_exists($_src_url)) {
                $src_url = $_src_url;
            }
            $_thumb_url = $thumbnailer->thumbPath($icdb->size, $icdb->md5, $icdb->mime);
            if (file_exists($_thumb_url)) {
                $thumb_url = $_thumb_url;
            }
            if (preg_match('/(\d+)x(\d+)/', $thumbnailer->calc($icdb->width, $icdb->height), $thumb_xy)) {
                $thumb_x = $thumb_xy[1];
                $thumb_y = $thumb_xy[2];
            }
            // �������L�^����Ă��Ȃ��Ƃ���DB���X�V
            if (isset($new_memo) && !strstr($icdb->memo, $new_memo)){
                $update = clone($icdb);
                if (!is_null($icdb->memo) && strlen($icdb->memo) > 0) {
                    $update->memo = $new_memo . ' ' . $icdb->memo;
                } else {
                    $update->memo = $new_memo;
                }
                $update->update();
                $img_memo = $update->memo;
            } elseif (!is_null($icdb->memo) && strlen($icdb->memo) > 0) {
                $img_memo = $icdb->memo;
            }

        // �L���b�V������Ă��Ȃ��Ƃ�
        } else {
            $src_url .= $append_memo;
            $thumb_url .= $append_memo;
        }

        $img = &new StdClass;
        $img->title     = $img_title;
        $img->src_url   = $src_url;
        $img->thumb_url = $thumb_url;
        $img->thumb_x   = $thumb_x;
        $img->thumb_y   = $thumb_y;
        $img->memo      = mb_convert_encoding($img_memo, 'SJIS-win', 'UTF-8');
        $images[] = $img;
    }

    $flexy->setData('images', $images);
    if ($isPopUp) {
        $flexy->setData('showForm', TRUE);
    }
} else {
    if (empty($isError) || $isPopUp) {
        $flexy->setData('showForm', TRUE);
    }
}

// }}}
// {{{ output


// �t�H�[�����e���v���[�g�p�I�u�W�F�N�g�ɕϊ�
$r = &new HTML_QuickForm_Renderer_ObjectFlexy($flexy);
//$r->setLabelTemplate('_label.tpl.html');
//$r->setHtmlTemplate('_html.tpl.html');
$qf->accept($r);
$qfObj = &$r->toObject();

// �ϐ���Assign
$flexy->setData('info_msg', $_info_msg_ht);
$flexy->setData('STYLE', $STYLE);
$flexy->setData('js', $qf->getValidationScript());
$flexy->setData('get', $qfObj);

// �y�[�W��\��
P2Util::header_nocache();
P2Util::header_content_type();
$flexy->compile('ic2g.tpl.html');
$flexy->output();


// }}}

?>