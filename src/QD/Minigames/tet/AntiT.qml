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
	<scene name="AntiT" game_name="АнтиТетрис">
		<animation name="back" flags="384" animation_file="Resource\AntiT\Animation\back.qda"/>
		<animation name="glass" flags="384" animation_file="Resource\AntiT\Animation\glass.qda"/>
		<animation name="yellow" flags="384" animation_file="Resource\AntiT\Animation\yellow.qda"/>
		<animation name="blue" flags="384" animation_file="Resource\AntiT\Animation\blue.qda"/>
		<animation name="crimson" flags="384" animation_file="Resource\AntiT\Animation\crimson.qda"/>
		<animation name="gold" flags="384" animation_file="Resource\AntiT\Animation\gold.qda"/>
		<animation name="green" flags="384" animation_file="Resource\AntiT\Animation\green.qda"/>
		<animation name="red" flags="384" animation_file="Resource\AntiT\Animation\red.qda"/>
		<animation name="transparent" flags="384" animation_file="Resource\AntiT\Animation\transparent0.qda"/>
		<animation name="fiolet" flags="384" animation_file="Resource\AntiT\Animation\fiolet.qda"/>
		<animation name="electric" flags="384" animation_file="Resource\AntiT\Animation\electric.qda"/>
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
			<pos_3d>0. 0. 0.</pos_3d>
			<object_state_static name="back">
				<screen_text_format global_depend="1"/>
				<animation_info animation="back"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>0. 0. 0.</default_pos>
		</animated_object>
		<animated_object name="cтакан">
			<pos_3d>0. 7.6293945e-006 -3.0517578e-005</pos_3d>
			<object_state_static name="glass">
				<screen_text_format global_depend="1"/>
				<animation_info animation="glass"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>0. 7.6293945e-006 -3.0517578e-005</default_pos>
		</animated_object>
		<animated_object name="квадрат">
			<pos_3d>-400. 400. 0.</pos_3d>
			<object_state_static name="transparent">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="yellow">
				<screen_text_format global_depend="1"/>
				<animation_info animation="yellow"/>
			</object_state_static>
			<object_state_static name="red">
				<screen_text_format global_depend="1"/>
				<animation_info animation="red"/>
			</object_state_static>
			<object_state_static name="green">
				<screen_text_format global_depend="1"/>
				<animation_info animation="green"/>
			</object_state_static>
			<object_state_static name="gold">
				<screen_text_format global_depend="1"/>
				<animation_info animation="gold"/>
			</object_state_static>
			<object_state_static name="crimson">
				<screen_text_format global_depend="1"/>
				<animation_info animation="crimson"/>
			</object_state_static>
			<object_state_static name="blue">
				<screen_text_format global_depend="1"/>
				<animation_info animation="blue"/>
			</object_state_static>
			<object_state_static name="electric">
				<screen_text_format global_depend="1"/>
				<animation_info animation="electric"/>
			</object_state_static>
			<object_state_static name="fiolet">
				<screen_text_format global_depend="1"/>
				<animation_info animation="fiolet"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-400. 400. 0.</default_pos>
		</animated_object>
		<animated_object name="state_flag">
			<pos_3d>-544.45837 616.71191 0.</pos_3d>
			<object_state_static name="game">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="win">
				<screen_text_format global_depend="1"/>
				<animation_info animation="blue"/>
			</object_state_static>
			<object_state_static name="lose">
				<screen_text_format global_depend="1"/>
				<animation_info animation="crimson"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-544.45837 616.71191 0.</default_pos>
		</animated_object>
	</scene>
	<trigger_chain name="Старт" trigger_bound="576 128 832 288" work_area="0 0 8576 8128" layout="-1 -1">
		<trigger_chain_root start_element="1" trigger_bound="576 128 832 160" trigger_element_title="Старт" trigger_element_cell_number="2 2">
			<child_links>
				<link ID="0" offsets="-21 16 -21 -16"/>
			</child_links>
		</trigger_chain_root>
		<trigger_element ID="0" trigger_bound="576 256 832 288" trigger_element_title="AntiT" trigger_element_cell_number="2 4">
			<named_object types="1 12">
				<name>AntiT</name>
			</named_object>
			<parent_links>
				<link ID="-1"/>
			</parent_links>
		</trigger_element>
	</trigger_chain>
	<minigame name="АнтиТетрис" config_file="AntiT_prm.ini" dll_name="dll\AntiT.dll">
		<minigame_config_prm name="back_object_name">фон</minigame_config_prm>
		<minigame_config_prm name="glass_object_name">cтакан</minigame_config_prm>
		<minigame_config_prm name="image_size">20 20</minigame_config_prm>
		<minigame_config_prm name="chip_object_name" size="500">квадрат</minigame_config_prm>
		<minigame_config_prm name="field_size">10 20</minigame_config_prm>
		<minigame_config_prm name="max_hieght_segment">3</minigame_config_prm>
		<minigame_config_prm name="crush_width">4</minigame_config_prm>
		<minigame_config_prm name="crush_height">3</minigame_config_prm>
		<minigame_config_prm name="figure_templates">antit.sha</minigame_config_prm>
	</minigame>
	<interface>
	</interface>
</qd_script>
