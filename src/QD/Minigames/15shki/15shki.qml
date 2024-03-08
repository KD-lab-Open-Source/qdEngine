<?xml version="1.0" encoding="WINDOWS-1251"?>
<qd_script>
	<game_title>пятнашки</game_title>
	<screen_text_format global_depend="0"/>
	<startup_scene>Новая сцена</startup_scene>
	<screen_size>800 600</screen_size>
	<text_set ID="0" pos_2d="320 240" screen_size="640 360"/>
	<mouse_object name="Мышь" default_cursors="0 0 0 0 0 0">
		<pos_3d>0. 0. 0.</pos_3d>
		<flag>32</flag>
		<bound>10. 10. 10.</bound>
		<default_pos>0. 0. 0.</default_pos>
	</mouse_object>
	<scene name="пятнашки" game_name="Пятнашки">
		<animation name="field1" flags="384" animation_file="Resource\пятнашки\Animation\field1.qda"/>
		<animation name="field2" flags="384" animation_file="Resource\пятнашки\Animation\field2.qda"/>
		<animation name="left" flags="384" animation_file="Resource\пятнашки\Animation\left.qda"/>
		<animation name="right" flags="384" animation_file="Resource\пятнашки\Animation\right.qda"/>
		<animation name="up" flags="384" animation_file="Resource\пятнашки\Animation\up.qda"/>
		<animation name="down" flags="384" animation_file="Resource\пятнашки\Animation\down.qda"/>
		<animation name="back" flags="384" animation_file="Resource\пятнашки\Animation\back.qda"/>
		<camera camera_grid_size="46 46">
			<camera_cell_size>32 32</camera_cell_size>
			<pos_3d>0 0 300.</pos_3d>
			<camera_focus>6000.</camera_focus>
			<camera_angles>1. 0. 0.</camera_angles>
			<camera_screen_size>800 600</camera_screen_size>
			<camera_screen_offset>5 -10</camera_screen_offset>
			<camera_screen_center>320 240</camera_screen_center>
			<camera_grid_center>0. 0. 0.</camera_grid_center>
			<camera_scale_pow>1.</camera_scale_pow>
			<camera_scale_z_offset>0.</camera_scale_z_offset>
		</camera>
		<animated_object name="пятнашка">
			<pos_3d>-282.1817 346.91245 0.</pos_3d>
			<object_state_static name="field1" flags="256" bound="20. 20. 0.">
				<screen_text_format global_depend="0"/>
				<animation_info animation="field1"/>
			</object_state_static>
			<object_state_static name="field2" flags="256" bound="20. 20. 0.">
				<screen_text_format global_depend="0"/>
				<animation_info animation="field2"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-282.1817 346.91245 0.</default_pos>
		</animated_object>
		<animated_object name="кнопка">
			<pos_3d>-336.0556 345.91211 0.</pos_3d>
			<object_state_static name="up" flags="256" bound="20. 20. 0.">
				<screen_text_format global_depend="0"/>
				<animation_info animation="up"/>
			</object_state_static>
			<object_state_static name="left" flags="256" bound="20. 20. 0.">
				<screen_text_format global_depend="0"/>
				<animation_info animation="left"/>
			</object_state_static>
			<object_state_static name="right" flags="256" bound="20. 20. 0.">
				<screen_text_format global_depend="0"/>
				<animation_info animation="right"/>
			</object_state_static>
			<object_state_static name="down" flags="256" bound="20. 20. 0.">
				<screen_text_format global_depend="0"/>
				<animation_info animation="down"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-336.0556 345.91211 0.</default_pos>
		</animated_object>
		<animated_object name="фон">
			<pos_3d>0. 3. -200.</pos_3d>
			<object_state_static name="back" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="back"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>0. 3. -200.</default_pos>
		</animated_object>
		<animated_object name="state_flag">
			<pos_3d>-496. 418. 0.</pos_3d>
			<object_state_static name="game">
				<screen_text_format global_depend="1"/>
				<animation_info animation="up"/>
			</object_state_static>
			<object_state_static name="win">
				<screen_text_format global_depend="1"/>
				<animation_info animation="down"/>
			</object_state_static>
			<object_state_static name="lose">
				<screen_text_format global_depend="1"/>
				<animation_info animation="right"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-496. 418. 0.</default_pos>
		</animated_object>
	</scene>
	<trigger_chain name="Новый триггер" trigger_bound="1728 512 1984 608" work_area="0 0 8000 8000" layout="-1 -1">
		<trigger_chain_root start_element="1" trigger_bound="1728 512 1984 544" trigger_element_title="Старт" trigger_element_cell_number="6 8">
			<child_links>
				<link ID="0" offsets="-20 16 -19 -17"/>
			</child_links>
		</trigger_chain_root>
		<trigger_element ID="0" trigger_bound="1728 576 1984 608" trigger_element_title="пятнашки" trigger_element_cell_number="6 9">
			<named_object types="1 12">
				<name>пятнашки</name>
			</named_object>
			<parent_links>
				<link ID="-1"/>
			</parent_links>
		</trigger_element>
	</trigger_chain>
	<minigame name="Пятнашки" config_file="15miniGame_par.ini" dll_name="DLL\15mini_game.dll">
		<minigame_config_prm name="butt_object_name" size="80">кнопка</minigame_config_prm>
		<minigame_config_prm name="field_object_name" size="400">пятнашка</minigame_config_prm>
		<minigame_config_prm name="back_object_name">фон</minigame_config_prm>
		<minigame_config_prm name="map_file_name" size="400">field.par</minigame_config_prm>
	</minigame>
	<interface>
	</interface>
</qd_script>
