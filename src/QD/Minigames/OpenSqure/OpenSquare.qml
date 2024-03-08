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
	<scene name="OpenSquare" game_name="OpenSquare">
		<animation name="line_up" flags="384" animation_file="Resource\OpenSquare\Animation\line_up.qda"/>
		<animation name="line_down" flags="384" animation_file="Resource\OpenSquare\Animation\line_down.qda"/>
		<animation name="line_left" flags="384" animation_file="Resource\OpenSquare\Animation\line_left.qda"/>
		<animation name="line_right" flags="384" animation_file="Resource\OpenSquare\Animation\line_right.qda"/>
		<animation name="transparent" flags="384" animation_file="Resource\OpenSquare\Animation\transparent.qda"/>
		<animation name="back" flags="384" animation_file="Resource\OpenSquare\Animation\back.qda"/>
		<animation name="filled" flags="384" animation_file="Resource\OpenSquare\Animation\filled.qda"/>
		<animation name="line_up_active" flags="384" animation_file="Resource\OpenSquare\Animation\line_up_active.qda"/>
		<animation name="line_down_active" flags="384" animation_file="Resource\OpenSquare\Animation\line_down_active.qda"/>
		<animation name="line_left_active" flags="384" animation_file="Resource\OpenSquare\Animation\line_left_active.qda"/>
		<animation name="line_right_active" flags="384" animation_file="Resource\OpenSquare\Animation\line_right_active.qda"/>
		<animation name="b" flags="384" animation_file="Resource\OpenSquare\Animation\b.qda"/>
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
		<animated_object name="бордюр">
			<pos_3d>-434.67551 572.78442 0.</pos_3d>
			<object_state_static name="transparent">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="line_up">
				<screen_text_format global_depend="1"/>
				<animation_info animation="line_up"/>
			</object_state_static>
			<object_state_static name="line_left">
				<screen_text_format global_depend="1"/>
				<animation_info animation="line_left"/>
			</object_state_static>
			<object_state_static name="line_right">
				<screen_text_format global_depend="1"/>
				<animation_info animation="line_right"/>
			</object_state_static>
			<object_state_static name="line_down">
				<screen_text_format global_depend="1"/>
				<animation_info animation="line_down"/>
			</object_state_static>
			<object_state_static name="line_up_active">
				<screen_text_format global_depend="1"/>
				<animation_info flags="4" animation="line_up_active"/>
			</object_state_static>
			<object_state_static name="line_left_active">
				<screen_text_format global_depend="1"/>
				<animation_info flags="4" animation="line_left_active"/>
			</object_state_static>
			<object_state_static name="line_right_active">
				<screen_text_format global_depend="1"/>
				<animation_info flags="4" animation="line_right_active"/>
			</object_state_static>
			<object_state_static name="line_down_active">
				<screen_text_format global_depend="1"/>
				<animation_info flags="4" animation="line_down_active"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-434.67551 572.78442 0.</default_pos>
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
		<animated_object name="тело">
			<pos_3d>-274.89484 572.31787 0.</pos_3d>
			<object_state_static name="transparent">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="filled">
				<screen_text_format global_depend="1"/>
				<animation_info animation="filled"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-274.89484 572.31787 0.</default_pos>
		</animated_object>
		<animated_object name="медвеженок">
			<pos_3d>-499.14578 45.499161 0.</pos_3d>
			<object_state_static name="b">
				<screen_text_format global_depend="1"/>
				<animation_info animation="b"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-499.14578 45.499161 0.</default_pos>
		</animated_object>
		<animated_object name="state_flag">
			<pos_3d>-538.07837 603.15253 0.</pos_3d>
			<object_state_static name="game">
				<screen_text_format global_depend="1"/>
				<animation_info animation="filled"/>
			</object_state_static>
			<object_state_static name="win">
				<screen_text_format global_depend="1"/>
				<animation_info animation="filled"/>
			</object_state_static>
			<object_state_static name="lose">
				<screen_text_format global_depend="1"/>
				<animation_info animation="filled"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-538.07837 603.15253 0.</default_pos>
		</animated_object>
	</scene>
	<trigger_chain name="старт-триггер" trigger_bound="576 128 832 288" work_area="0 0 8000 8000" layout="-1 -1">
		<trigger_chain_root start_element="1" trigger_bound="576 128 832 160" trigger_element_title="Старт" trigger_element_cell_number="2 2">
			<child_links>
				<link ID="0" offsets="-18 15 -18 -16"/>
			</child_links>
		</trigger_chain_root>
		<trigger_element ID="0" trigger_bound="576 256 832 288" trigger_element_title="OpenSquare" trigger_element_cell_number="2 4">
			<named_object types="1 12">
				<name>OpenSquare</name>
			</named_object>
			<parent_links>
				<link ID="-1"/>
			</parent_links>
		</trigger_element>
	</trigger_chain>
	<minigame name="OpenSquare" config_file="OpenSquare_prm.ini" dll_name="dll\OpenSquare.dll">
		<minigame_config_prm name="map_file_name">level.par</minigame_config_prm>
		<minigame_config_prm name="back_object_name">фон</minigame_config_prm>
		<minigame_config_prm name="image_size">40 40</minigame_config_prm>
		<minigame_config_prm name="border_object_name" size="400">бордюр</minigame_config_prm>
		<minigame_config_prm name="body_object_name" size="500">тело</minigame_config_prm>
		<minigame_config_prm name="field_size">10 10</minigame_config_prm>
		<minigame_config_prm name="win_zone">2 2 4 5</minigame_config_prm>
		<minigame_config_prm name="win_image">медвеженок</minigame_config_prm>
	</minigame>
	<interface>
	</interface>
</qd_script>
