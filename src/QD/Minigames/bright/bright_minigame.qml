<?xml version="1.0" encoding="WINDOWS-1251"?>
<qd_script>
	<screen_text_format global_depend="0"/>
	<screen_size>800 600</screen_size>
	<text_set ID="0" pos_2d="320 240" screen_size="640 360"/>
	<mouse_object name="����" default_cursors="0 0 0 0 0 0">
		<pos_3d>0. 0. 0.</pos_3d>
		<flag>32</flag>
		<bound>10. 10. 10.</bound>
		<default_pos>0. 0. 0.</default_pos>
	</mouse_object>
	<scene name="�������" game_name="�������">
		<animation name="������" flags="384" animation_file="Resource\�������\Animation\������.qda"/>
		<animation name="��������������" flags="384" animation_file="Resource\�������\Animation\��������������.qda"/>
		<animation name="back" flags="384" animation_file="Resource\�������\Animation\back.qda"/>
		<animation name="bw_white" flags="384" animation_file="Resource\�������\Animation\bw_white.qda"/>
		<animation name="bw_black" flags="384" animation_file="Resource\�������\Animation\bw_black.qda"/>
		<animation name="transparent" flags="384" animation_file="Resource\�������\Animation\transparent.qda"/>
		<animation name="light_gray" flags="384" animation_file="Resource\�������\Animation\light_gray.qda"/>
		<animation name="dark_gray" flags="384" animation_file="Resource\�������\Animation\dark_gray.qda"/>
		<animation name="good" flags="384" animation_file="Resource\�������\Animation\good.qda"/>
		<animation name="plus" flags="384" animation_file="Resource\�������\Animation\plus.qda"/>
		<animation name="minus" flags="384" animation_file="Resource\�������\Animation\minus.qda"/>
		<animation name="test" flags="384" animation_file="Resource\�������\Animation\test.qda"/>
		<camera camera_grid_size="46 46">
			<camera_cell_size>32 32</camera_cell_size>
			<pos_3d>0 0 300.</pos_3d>
			<camera_focus>6000.</camera_focus>
			<camera_angles>10. 0. 0.</camera_angles>
			<camera_screen_size>800 600</camera_screen_size>
			<camera_screen_offset>0 0</camera_screen_offset>
			<camera_screen_center>320 240</camera_screen_center>
			<camera_grid_center>0. 0. 0.</camera_grid_center>
			<camera_scale_pow>1.</camera_scale_pow>
			<camera_scale_z_offset>0.</camera_scale_z_offset>
		</camera>
		<animated_object name="������">
			<pos_3d>-400. 400. 0.</pos_3d>
			<object_state_static name="������">
				<screen_text_format global_depend="1"/>
				<animation_info animation="������"/>
			</object_state_static>
			<object_state_static name="��������������">
				<screen_text_format global_depend="1"/>
				<animation_info flags="4" animation="��������������"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-400. 400. 0.</default_pos>
		</animated_object>
		<animated_object name="���">
			<pos_3d>0. 0. 0.</pos_3d>
			<object_state_static name="back">
				<screen_text_format global_depend="1"/>
				<animation_info animation="back"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>0. 0. 0.</default_pos>
		</animated_object>
		<animated_object name="������_�����_��������">
			<pos_3d>-320.6145 401.88077 0.</pos_3d>
			<object_state_static name="bw_trans">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="bw_black">
				<screen_text_format global_depend="1"/>
				<animation_info animation="bw_black"/>
			</object_state_static>
			<object_state_static name="bw_white">
				<screen_text_format global_depend="1"/>
				<animation_info animation="bw_white"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-320.6145 401.88077 0.</default_pos>
		</animated_object>
		<animated_object name="��������000">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="��������001">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="��������002">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="��������003">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="��������004">
			<pos_3d>-236. 399. 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-236. 399. 0.</default_pos>
		</animated_object>
		<animated_object name="��������005">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="��������006">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="��������007">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="��������008">
			<pos_3d>-236. 399. 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-236. 399. 0.</default_pos>
		</animated_object>
		<animated_object name="��������009">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="��������010">
			<pos_3d>-236. 399. 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-236. 399. 0.</default_pos>
		</animated_object>
		<animated_object name="��������011">
			<pos_3d>-235.66249 398.61115 0.</pos_3d>
			<object_state_static name="dark_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="dark_gray"/>
			</object_state_static>
			<object_state_static name="good">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="light_gray">
				<screen_text_format global_depend="1"/>
				<animation_info animation="light_gray"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-235.66249 398.61115 0.</default_pos>
		</animated_object>
		<animated_object name="�����������">
			<pos_3d>-142.23405 395.34222 0.</pos_3d>
			<object_state_static name="transparent">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="plus">
				<screen_text_format global_depend="1"/>
				<animation_info animation="plus"/>
			</object_state_static>
			<object_state_static name="minus">
				<screen_text_format global_depend="1"/>
				<animation_info animation="minus"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-142.23405 395.34222 0.</default_pos>
		</animated_object>
		<animated_object name="state_flag">
			<pos_3d>-504.30774 404.06079 0.</pos_3d>
			<object_state_static name="game">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<object_state_static name="win">
				<screen_text_format global_depend="1"/>
				<animation_info animation="plus"/>
			</object_state_static>
			<object_state_static name="lose">
				<screen_text_format global_depend="1"/>
				<animation_info animation="minus"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-504.30774 404.06079 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble000">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble001">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble002">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble003">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble004">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble005">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble006">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble007">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble008">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble009">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble010">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="ShiftAble011">
			<pos_3d>-69.006432 402.09955 0.</pos_3d>
			<object_state_static name="off" flags="256" bound="3. 3. 3.">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="test"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-69.006432 402.09955 0.</default_pos>
		</animated_object>
		<animated_object name="CurrentActive">
			<pos_3d>-104.02811 397.74026 0.</pos_3d>
			<object_state_static name="off">
				<screen_text_format global_depend="1"/>
				<animation_info animation="transparent"/>
			</object_state_static>
			<object_state_static name="on">
				<screen_text_format global_depend="1"/>
				<animation_info animation="good"/>
			</object_state_static>
			<bound>10. 10. 10.</bound>
			<default_pos>-104.02811 397.74026 0.</default_pos>
		</animated_object>
	</scene>
	<trigger_chain name="�����-�������" trigger_bound="576 128 832 288" work_area="0 0 8000 8000" layout="-1 -1">
		<trigger_chain_root start_element="1" trigger_bound="576 128 832 160" trigger_element_title="�����" trigger_element_cell_number="2 2">
			<child_links>
				<link ID="0" offsets="-19 16 -19 -17"/>
			</child_links>
		</trigger_chain_root>
		<trigger_element ID="0" trigger_bound="576 256 832 288" trigger_element_title="�������" trigger_element_cell_number="2 4">
			<named_object types="1 12">
				<name>�������</name>
			</named_object>
			<parent_links>
				<link ID="-1"/>
			</parent_links>
		</trigger_element>
	</trigger_chain>
	<minigame name="�������" config_file="bright_minigame_par.ini" dll_name="dll\bright.dll">
		<minigame_config_prm name="back_object_name">���</minigame_config_prm>
		<minigame_config_prm name="square_object_name" size="50">������</minigame_config_prm>
		<minigame_config_prm name="able_object_name">ShiftAble</minigame_config_prm>
		<minigame_config_prm name="modify_object_name" size="50">�����������</minigame_config_prm>
		<minigame_config_prm name="map_file_name">bright_level.par</minigame_config_prm>
		<minigame_config_prm name="chip_name_bw" size="50">������_�����_��������</minigame_config_prm>
		<minigame_config_prm name="chip_name_000">��������000</minigame_config_prm>
		<minigame_config_prm name="chip_name_001">��������001</minigame_config_prm>
		<minigame_config_prm name="chip_name_002">��������002</minigame_config_prm>
		<minigame_config_prm name="chip_name_003">��������003</minigame_config_prm>
		<minigame_config_prm name="chip_name_004">��������004</minigame_config_prm>
		<minigame_config_prm name="chip_name_005">��������005</minigame_config_prm>
		<minigame_config_prm name="chip_name_006">��������006</minigame_config_prm>
		<minigame_config_prm name="chip_name_007">��������007</minigame_config_prm>
		<minigame_config_prm name="chip_name_008">��������008</minigame_config_prm>
		<minigame_config_prm name="chip_name_009">��������009</minigame_config_prm>
		<minigame_config_prm name="chip_name_010">��������010</minigame_config_prm>
		<minigame_config_prm name="chip_name_011">��������011</minigame_config_prm>
		<minigame_config_prm name="field_size">4 3</minigame_config_prm>
		<minigame_config_prm name="states_count">3</minigame_config_prm>
	</minigame>
	<interface>
		<interface_screen name="�������">
		</interface_screen>
	</interface>
</qd_script>
