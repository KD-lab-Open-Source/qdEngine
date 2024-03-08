<?xml version="1.0" encoding="WINDOWS-1251"?>
<qd_script>
	<screen_text_format global_depend="0"/>
	<screen_size>800 600</screen_size>
	<text_set ID="0" pos_2d="320 240" screen_size="640 360"/>
	<mouse_object name="Мышь" default_cursors="0 0 0 0 0 0">
		<pos_3d>0. 0. 0.</pos_3d>
		<flag>32</flag>
		<bound>10. 10. 10.</bound>
		<default_pos>0. 0. 0.</default_pos>
	</mouse_object>
	<scene name="полигоны" game_name="Полигон">
		<animation name="pole" flags="384" animation_file="Resource\полигоны\Animation\back.qda"/>
		<animation name="1" flags="384" animation_file="Resource\полигоны\Animation\1.qda"/>
		<animation name="2" flags="384" animation_file="Resource\полигоны\Animation\2.qda"/>
		<animation name="3" flags="384" animation_file="Resource\полигоны\Animation\3.qda"/>
		<animation name="4" flags="384" animation_file="Resource\полигоны\Animation\4.qda"/>
		<animation name="5" flags="384" animation_file="Resource\полигоны\Animation\5.qda"/>
		<animation name="6" flags="384" animation_file="Resource\полигоны\Animation\6.qda"/>
		<animation name="7" flags="384" animation_file="Resource\полигоны\Animation\7.qda"/>
		<animation name="8" flags="384" animation_file="Resource\полигоны\Animation\8.qda"/>
		<animation name="9" flags="384" animation_file="Resource\полигоны\Animation\9.qda"/>
		<animation name="10" flags="384" animation_file="Resource\полигоны\Animation\10.qda"/>
		<animation name="11" flags="384" animation_file="Resource\полигоны\Animation\11.qda"/>
		<animation name="polygonActive" flags="384" animation_file="resource\Полигоны\Animation\polygonActive2.qda"/>
		<animation name="polygon" flags="384" animation_file="resource\Полигоны\Animation\polygon.qda"/>
		<camera camera_grid_size="46 46">
			<camera_cell_size>32 32</camera_cell_size>
			<pos_3d>0 0 300.</pos_3d>
			<camera_focus>6000.</camera_focus>
			<camera_angles>45. 0. 0.</camera_angles>
			<camera_screen_size>800 600</camera_screen_size>
			<camera_screen_offset>0 0</camera_screen_offset>
			<camera_screen_center>320 240</camera_screen_center>
			<camera_grid_center>0. 0. 0.</camera_grid_center>
			<camera_scale_pow>1.</camera_scale_pow>
			<camera_scale_z_offset>0.</camera_scale_z_offset>
		</camera>
		<animated_object name="фон">
			<pos_3d>0. 1.5258789e-005 -3.0517578e-005</pos_3d>
			<object_state_static name="pole">
				<screen_text_format global_depend="1"/>
				<animation_info animation="pole"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>0. 1.5258789e-005 -3.0517578e-005</default_pos>
		</animated_object>
		<animated_object name="фишка">
			<pos_3d>-397.31787 273.00049 263.62146</pos_3d>
			<object_state_static name="1">
				<screen_text_format global_depend="1"/>
				<animation_info animation="1"/>
			</object_state_static>
			<object_state_static name="2">
				<screen_text_format global_depend="1"/>
				<animation_info animation="2"/>
			</object_state_static>
			<object_state_static name="3">
				<screen_text_format global_depend="1"/>
				<animation_info animation="3"/>
			</object_state_static>
			<object_state_static name="4">
				<screen_text_format global_depend="1"/>
				<animation_info animation="4"/>
			</object_state_static>
			<object_state_static name="5">
				<screen_text_format global_depend="1"/>
				<animation_info animation="5"/>
			</object_state_static>
			<object_state_static name="6">
				<screen_text_format global_depend="1"/>
				<animation_info animation="6"/>
			</object_state_static>
			<object_state_static name="7">
				<screen_text_format global_depend="1"/>
				<animation_info animation="7"/>
			</object_state_static>
			<object_state_static name="8">
				<screen_text_format global_depend="1"/>
				<animation_info animation="8"/>
			</object_state_static>
			<object_state_static name="9">
				<screen_text_format global_depend="1"/>
				<animation_info animation="9"/>
			</object_state_static>
			<object_state_static name="10">
				<screen_text_format global_depend="1"/>
				<animation_info animation="10"/>
			</object_state_static>
			<object_state_static name="11">
				<screen_text_format global_depend="1"/>
				<animation_info animation="11"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-397.31787 273.00049 263.62146</default_pos>
		</animated_object>
		<animated_object name="полигон">
			<pos_3d>-398.36896 273.00049 263.62146</pos_3d>
			<object_state_static name="polygon">
				<screen_text_format global_depend="1"/>
				<animation_info animation="polygon"/>
			</object_state_static>
			<object_state_static name="polygonActive">
				<screen_text_format global_depend="1"/>
				<animation_info flags="4" animation="polygonActive"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-398.36896 273.00049 263.62146</default_pos>
		</animated_object>
		<animated_object name="state_flag">
			<pos_3d>-537.36621 630.31 0.</pos_3d>
			<object_state_static name="game">
				<screen_text_format global_depend="1"/>
				<animation_info animation="1"/>
			</object_state_static>
			<object_state_static name="win">
				<screen_text_format global_depend="1"/>
				<animation_info animation="2"/>
			</object_state_static>
			<object_state_static name="lose">
				<screen_text_format global_depend="1"/>
				<animation_info animation="3"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-537.36621 630.31 0.</default_pos>
		</animated_object>
	</scene>
	<trigger_chain name="Старт-триггер" trigger_bound="1152 576 1408 736" work_area="0 0 8000 8000" layout="-1 -1">
		<trigger_chain_root start_element="1" trigger_bound="1152 576 1408 608" trigger_element_title="Старт" trigger_element_cell_number="4 9">
			<child_links>
				<link ID="0" offsets="-10 16 -10 -16"/>
			</child_links>
		</trigger_chain_root>
		<trigger_element ID="0" trigger_bound="1152 704 1408 736" trigger_element_title="полигоны" trigger_element_cell_number="4 11">
			<named_object types="1 12">
				<name>полигоны</name>
			</named_object>
			<parent_links>
				<link ID="-1"/>
			</parent_links>
		</trigger_element>
	</trigger_chain>
	<minigame name="Полигон" config_file="Polygon_miniGame_par.ini" dll_name="dll\MinigamePolygon.dll">
		<minigame_config_prm name="back_object_name">фон</minigame_config_prm>
		<minigame_config_prm name="poly_object_name" size="400">полигон</minigame_config_prm>
		<minigame_config_prm name="chip_object_name" size="400">фишка</minigame_config_prm>
		<minigame_config_prm name="map_file_name" size="400">map.par</minigame_config_prm>
	</minigame>
	<interface>
		<interface_screen name="Полигоны">
		</interface_screen>
	</interface>
</qd_script>
