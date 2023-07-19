#include "editor/editor.hpp"

#include <array>
#include <string_view>

#include <imgui.h>

#include "editor/camera.hpp"

namespace pine {

Editor::Editor() : camera_controller(1.0f, 1.0f) {}

void Editor::init()
{
    const auto font = pine::font::get_default();
    const auto& gui = Engine::get().get_gui();
    gui.load_font(font.data(), font.size(), 18.0f);

    // Load shader
    auto shader = create_shader("resources/shaders/quad.vert",
        "resources/shaders/quad.frag");

    const auto shader_loaded
        = shader_library.load_shader("resources/shaders/quad.vert",
            "resources/shaders/quad.frag");

    if (!shader_loaded)
        PINE_ERROR("Failed to load shader.");

    FramebufferSpecs specs;
    specs.width = 1;
    specs.height = 1;
    specs.color_attachment.format = TextureFormat::RGBA;
    specs.color_attachment.filter = TextureFilter::LINEAR;
    specs.depth_attachment.format = TextureFormat::DEPTH32F;

    viewport_framebuffer = create_framebuffer(specs);

    quad_render_data = QuadRenderer::init();

    server.set_connection_callback(
        [](const ConnectionState& connection) -> bool {
            PINE_INFO("Editor server: New connection {0}",
                connection.socket.remote_endpoint());
            return true;
        });

    server.set_message_callback(
        [this](const std::vector<uint8_t>& message) -> void
        { server_history.push_back(message); });

    start_server(server);
}

void Editor::shutdown() {}

void Editor::update(const Timestep& ts)
{
    if (viewport_window.is_focused()) {
        update_camera_controller(ts);
    }

    quad_rotation += ts * 50.0f;

    RenderCommand::set_clear_color({0.1f, 0.1f, 0.1f, 1.0f});
    RenderCommand::clear();

    viewport_framebuffer->bind();
    RenderCommand::set_clear_color({0.05f, 0.05f, 0.05f, 1.0f});
    RenderCommand::clear();

    QuadRenderer::begin_scene(quad_render_data, camera_controller.get_camera());

    QuadRenderer::draw_rotated_quad(quad_render_data,
        {0.0f, 0.0f},
        {0.8f, 0.8f},
        radians(quad_rotation),
        {0.9f, 0.1f, 0.2f, 1.0f});
    QuadRenderer::draw_quad(quad_render_data,
        {2.0f, -2.0f},
        {0.8f, 0.8f},
        {0.8f, 0.2f, 0.3f, 1.0f});
    QuadRenderer::draw_quad(quad_render_data,
        {2.0f, 2.0f},
        {0.5f, 0.75f},
        quad_color);

    if (texture)
    {
        QuadRenderer::draw_quad(quad_render_data,
            {-16.0f, 0.0f, -0.2f},
            {16.0f, 9.0f},
            texture,
            1.0f,
            Vec4{1.0f, 1.0f, 1.0f, 1.0f});
    }

    QuadRenderer::end_scene(quad_render_data);
    viewport_framebuffer->unbind();

    update_server(server);
}

void Editor::on_gui_render()
{
    gui::render_dockspace("editor_dockspace");
    gui::main_menu_bar(
        []()
        {
            static bool show_imgui_demo_window = false;
            static bool show_imgui_metrics = false;
            static bool show_imgui_stack_tool = false;
            static bool show_imgui_style_editor = false;
            static bool show_filesystem_popup = false;

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                }
                if (ImGui::MenuItem("Exit", "Ctrl+W"))
                {
                    Engine::get().stop();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z"))
                {
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y"))
                {
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Directory"))
            {
                if (ImGui::MenuItem("Working directory"))
                {
                    show_filesystem_popup = true;
                }
                if (ImGui::MenuItem("Resource directory"))
                {
                    // TODO: Render resources directory panel.
                }
                ImGui::EndMenu();
            }

            if (show_filesystem_popup)
                ImGui::OpenPopup("WorkingDirectory");

            if (ImGui::BeginPopupModal("WorkingDirectory"))
            {
                static std::array<char, 256> working_directory_buffer = {""};
                strcpy(working_directory_buffer.data(),
                    filesystem::get_working_directory().string().c_str());
                ImGui::Text("Working directory: %s",
                    working_directory_buffer.data());
                if (ImGui::Button("Close"))
                {
                    show_filesystem_popup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginMenu("Gui"))
            {
                ImGui::Checkbox("Show demo window", &show_imgui_demo_window);
                ImGui::Checkbox("Show metrics", &show_imgui_metrics);
                ImGui::Checkbox("Show stack tool", &show_imgui_stack_tool);
                ImGui::Checkbox("Show style editor", &show_imgui_style_editor);
                ImGui::EndMenu();
            }

            if (show_imgui_demo_window)
                ImGui::ShowDemoWindow();
            if (show_imgui_metrics)
                ImGui::ShowMetricsWindow();
            if (show_imgui_stack_tool)
                ImGui::ShowStackToolWindow();
            if (show_imgui_style_editor)
            {
                ImGui::Begin("Dear ImGui Style Editor",
                    &show_imgui_style_editor);
                ImGui::ShowStyleEditor();
                ImGui::End();
            }
        });

    viewport_window.on_render(
        [this]()
        {
            const auto texture_id
                = viewport_framebuffer->get_color_attachment_renderer_id();
            const auto size = ImGui::GetContentRegionAvail();

            ImGui::Image(reinterpret_cast<void*>(texture_id),
                ImVec2{size.x, size.y},
                ImVec2{0, 1},
                ImVec2{1, 0});
        });

    update_viewport();

    Engine::get().get_gui().block_events(
        !viewport_window.is_focused() || !viewport_window.is_hovered());

    gui_control_window.on_render(
        []()
        {
            static std::array<char, 50> input_profile_name{""};

            ImGui::InputText("GUI Profile",
                input_profile_name.data(),
                input_profile_name.size());

            const auto& gui = Engine::get().get_gui();

            if (ImGui::Button("Load profile"))
            {
                const std::string_view filename(input_profile_name.data(),
                    input_profile_name.size());
                const std::filesystem::path filepath = filename;
                gui.load_settings(filepath);
            }

            if (ImGui::Button("Save profile"))
            {
                const std::string_view filename(input_profile_name.data(),
                    input_profile_name.size());
                const std::filesystem::path filepath = filename;
                gui.save_settings(filepath);
            }
        });

    network_window.on_render(
        [this]()
        {
            static std::array<char, 256> remote_address = {""};
            static uint16_t remote_port = 0;
            ImGui::InputText("Address",
                remote_address.data(),
                remote_address.size());
            gui::input_scalar("Remote port", &remote_port);
            ImGui::Text("Client connected: %d", is_connected(client));

            if (ImGui::Button("Connect"))
            {
                const auto address
                    = std::string(remote_address.data(), remote_address.size());
                connect(client, address, remote_port);
            }
            ImGui::SameLine();
            if (ImGui::Button("Disconnect"))
            {
                disconnect(client);
            }

            static constexpr size_t message_size = 10 * 5;
            static char message_text[message_size] = "";
            ImGui::InputTextMultiline("Message",
                message_text,
                IM_ARRAYSIZE(message_text),
                ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 5),
                ImGuiInputTextFlags_AllowTabInput);

            if (ImGui::Button("Send to server"))
            {
                if (is_connected(client))
                {
                    const auto message = std::vector<uint8_t>(message_text,
                        message_text + sizeof(message_text));
                    send(client, message.data(), message.size());
                }
            }

            ImGui::Separator();

            ImGui::Text("Server");

            for (const auto& connection : server.connections)
            {
                const auto endpoint = connection->socket.remote_endpoint();
                ImGui::Text("Connection: %s:%d",
                    endpoint.address().to_string().c_str(),
                    endpoint.port());
            }

            gui::empty_space(0.0f, 20.0f);

            ImGui::Text("Server messages:");
            for (const auto& message : server_history)
            {
                const auto message_text
                    = std::string(message.begin(), message.end());
                ImGui::Text("%s", message_text.c_str());
            }
        });

    renderer_window.on_render(
        [this]()
        {
            auto& stats = quad_render_data.statistics;
            ImGui::Text("QuadRenderer Stats:");
            ImGui::Text("Draw Calls: %d", stats.draw_calls);
            ImGui::Text("Quads: %d", stats.quad_count);
            ImGui::Text("Vertices: %d", stats.get_total_vertex_count());
            ImGui::Text("Indices: %d", stats.get_total_index_count());

            ImGui::ColorEdit4("Square Color", value_ptr(quad_color));

            gui::empty_space(0.0f, 10.0f);
            ImGui::Separator();

            for (const auto& [name, shader] : shader_library.get_shader_map())
            {
                ImGui::Text("%s", name.c_str());
            }

            gui::empty_space(0.0f, 10.0f);
            ImGui::Separator();

            static bool flip_image = false;
            static std::array<char, 256> image_path = {""};
            static auto image_format = ImageFormat::BGRA;

            static constexpr std::array<std::pair<const char*, ImageFormat>, 6>
                image_format_options
                = {std::make_pair("Gray", ImageFormat::GRAY),
                    std::make_pair("Gray-alpha", ImageFormat::GRAY_ALPHA),
                    std::make_pair("RGB", ImageFormat::RGB),
                    std::make_pair("BGR", ImageFormat::BGR),
                    std::make_pair("RGBA", ImageFormat::RGBA),
                    std::make_pair("BGRA", ImageFormat::BGRA)};

            ImGui::InputText("Image path",
                image_path.data(),
                image_path.size());

            gui::dropdown("Image format", &image_format, image_format_options);

            ImGui::Checkbox("Flip image", &flip_image);
            ImGui::SameLine();
            if (ImGui::Button("Load image"))
            {
                if (filesystem::is_file(image_path.data()))
                {
                    texture = Texture2D::create(read_image(image_path.data(),
                        image_format,
                        flip_image));
                    PINE_INFO("Loaded image: {0}, {1}",
                        image_path.data(),
                        image_format);
                }
            }

            gui::empty_space(0.0f, 20.0f);
            ImGui::Separator();
        });

    console_window.on_render();

    test_window.on_render(
        [this]()
        {
            const auto position = test_window.get_position();
            const auto size = test_window.get_size();
            const auto focused = test_window.is_focused();
            const auto hovered = test_window.is_hovered();
            ImGui::Text("Position: %f, %f", position.x, position.y);
            ImGui::Text("Size:     %f, %f", size.x, size.y);
            ImGui::Text("Focused:  %d", focused);
            ImGui::Text("Hovered:  %d", hovered);

            ImGui::Separator();

            for (const auto uuid : uuids)
            {
                ImGui::Text("UUID:\t%s", uuid.to_string().c_str());
            }
        });

    camera_window.on_render(
        [this](){
            render_camera_controls(camera_controller);
        }
    );
}

void Editor::on_event(const Event& event) { 
    if (viewport_window.is_focused() && viewport_window.is_hovered())
    {
        dispatch_event<Moved<MouseWheel>>(event,
            [this](const Moved<MouseWheel>& event){
                camera_controller.increment_zoom(event.source.offset_y);
            }
        );

    }
}

void Editor::update_camera_controller(const Timestep& timestep)
{
    const auto window = Engine::get().get_window();
    if (window)
    {
        const auto input_handle = InputHandle::create(*window.get());

        if (input_handle->is_key_pressed(KeyCode::A))
            camera_controller.move_left(timestep);
        if (input_handle->is_key_pressed(KeyCode::D))
            camera_controller.move_right(timestep);
        if (input_handle->is_key_pressed(KeyCode::W))
            camera_controller.move_up(timestep);
        if (input_handle->is_key_pressed(KeyCode::S))
            camera_controller.move_down(timestep);
        if (input_handle->is_key_pressed(KeyCode::Q))
            camera_controller.rotate_counter_clockwise(timestep);
        if (input_handle->is_key_pressed(KeyCode::E))
            camera_controller.rotate_clockwise(timestep);
    }
    camera_controller.update_camera();
}

void Editor::update_viewport()
{
    const auto& specs = viewport_framebuffer->get_specification();
    const auto size = viewport_window.get_size();

    camera_controller.set_aspect(size.x, size.y);
    camera_controller.update_camera();

    const auto viewport_valid = size.x > 0.0f && size.y > 0.0f;
    const auto viewport_changed = static_cast<float>(specs.width) != size.x
        || static_cast<float>(specs.height) != size.y;
    if (viewport_valid && viewport_changed)
    {
        viewport_framebuffer->resize(static_cast<uint32_t>(size.x),
            static_cast<uint32_t>(size.y));
    }
}

} // namespace pine
