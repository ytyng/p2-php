<?php
// p2 �[�X�^�C���ݒ�
// for menu

echo <<<EOP

<style type="text/css" media="all">
<!--
	body{
		line-height:136%;
		background-color:{$STYLE['menu_bgcolor']};
		font-size:{$STYLE['menu_fontsize']};
		color:{$STYLE['menu_color']};
	}
	
	div.menu_cate{margin:0.5em 0;} /* �J�e�S���[ */
	
	/* a{text-decoration:none;} */
	a:link.menu_cate{color:{$STYLE['menu_cate_color']};}
	a:visited.menu_cate{color:{$STYLE['menu_cate_color']};}
	a:hover.menu_cate{color:{$STYLE['menu_cate_color']};}
	
	.itas a:link{color:{$STYLE['menu_ita_color']};} /* �� �����N */
	.itas a:visited{color:{$STYLE['menu_ita_color_v']};}
	.itas a:hover{color:{$STYLE['menu_ita_color_h']};}
	.itas_hide a:link{color:{$STYLE['menu_ita_color']};}
	.itas_hide a:visited{color:{$STYLE['menu_ita_color_v']};}
	.itas_hide a:hover{color:{$STYLE['menu_ita_color_h']};}
	/* .itas_hide{display:none;} js/showHide.js��*/
	
	a:link.fav{color:{$STYLE['fav_color']};} /* ���C�Ƀ}�[�N */
	a:visited.fav{color:{$STYLE['fav_color']};}
	a:hover.fav{color:{$STYLE['acolor_h']};}
	
	a:link.newres_num{color:{$STYLE['menu_newres_color']};} /* �V�����X�� */
	a:visited.newres_num{color:{$STYLE['menu_newres_color']};}
	a:hover.newres_num{color:{$STYLE['menu_newres_color']};}
	
	a:link.newres_num_zero{color:{$STYLE['menu_color']};} /* �V�����X���[�� */
	a:visited.newres_num_zero{color:{$STYLE['menu_color']};}
	a:hover.newres_num_zero{color:{$STYLE['menu_color']};}
	
	.newthre_num{color:{$STYLE['menu_newthre_color']};}	/* �V�K�X���b�h�� */
	
-->
</style>

EOP;
?>