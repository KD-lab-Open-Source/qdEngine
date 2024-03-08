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
	<scene name="Родители" game_name="Родители">
		<animation name="1" flags="384" animation_file="Resource\Родители\Animation\1.qda"/>
		<animation name="2" flags="384" animation_file="Resource\Родители\Animation\2.qda"/>
		<animation name="3" flags="384" animation_file="Resource\Родители\Animation\3.qda"/>
		<animation name="4" flags="384" animation_file="Resource\Родители\Animation\4.qda"/>
		<animation name="5" flags="384" animation_file="Resource\Родители\Animation\5.qda"/>
		<animation name="6" flags="384" animation_file="Resource\Родители\Animation\6.qda"/>
		<animation name="back" flags="384" animation_file="resource\Родители\Animation\back.qda"/>
		<animation name="activ" flags="384" animation_file="resource\Родители\Animation\bord.qda"/>
		<animation name="b0" flags="384" animation_file="Resource\Родители\Animation\b0.qda"/>
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
		<animated_object name="фрагмент">
			<pos_3d>-420. 570. 0.</pos_3d>
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
			<bound>10. 10. 10.</bound>
			<default_pos>-420. 570. 0.</default_pos>
		</animated_object>
		<animated_object name="фон">
			<pos_3d>0. 0. 0.</pos_3d>
			<object_state_static name="back">
				<screen_text_format global_depend="1"/>
				<animation_info animation="back"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>0. 0. 0.</default_pos>
		</animated_object>
		<animated_object name="bord">
			<pos_3d>-420. 570. 0.</pos_3d>
			<object_state_static name="b0">
				<screen_text_format global_depend="1"/>
				<animation_info animation="b0"/>
			</object_state_static>
			<object_state_static name="activ">
				<screen_text_format global_depend="1"/>
				<animation_info flags="4" animation="activ"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-420. 570. 0.</default_pos>
		</animated_object>
		<animated_object name="state_flag">
			<pos_3d>-547.3385 608.2326 0.</pos_3d>
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
			<default_pos>-547.3385 608.2326 0.</default_pos>
		</animated_object>
	</scene>
	<trigger_chain name="Старт" trigger_bound="576 484 832 580" work_area="0 0 8576 8420" layout="-1 -1">
		<trigger_chain_root start_element="1" trigger_bound="576 484 832 516" trigger_element_title="Старт" trigger_element_cell_number="2 7">
			<child_links>
				<link ID="0" offsets="-18 15 -18 -17"/>
			</child_links>
		</trigger_chain_root>
		<trigger_element ID="0" trigger_bound="576 548 832 580" trigger_element_title="Родители" trigger_element_cell_number="2 8">
			<named_object types="1 12">
				<name>Родители</name>
			</named_object>
			<parent_links>
				<link ID="-1"/>
			</parent_links>
		</trigger_element>
	</trigger_chain>
	<minigame name="Родители" config_file="parent_prm.ini" dll_name="dll\ParentMinigame.dll">
		<minigame_config_prm name="map_file_name">level.par</minigame_config_prm>
		<minigame_config_prm name="back_object_name">фон</minigame_config_prm>
		<minigame_config_prm name="chip_object_name" size="20">фрагмент</minigame_config_prm>
		<minigame_config_prm name="bord_object_name" size="20">bord</minigame_config_prm>
	</minigame>
	<interface>
	</interface>
</qd_script>
