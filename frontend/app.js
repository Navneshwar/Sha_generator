document.addEventListener('DOMContentLoaded', async () => {
    const inputText = document.getElementById('inputText');
    const generateBtn = document.getElementById('generateBtn');
    const outputHash = document.getElementById('outputHash');
    const status = document.getElementById('status');

    status.textContent = "Loading SHA-0...";
    generateBtn.disabled = true;

    try {
        // Load Emscripten's glue code
        const script = document.createElement('script');
        script.src = 'wasm/sha0.js';
        document.body.appendChild(script);

        script.onload = async () => {
            // Wait for WASM initialization
            await new Promise(resolve => {
                Module.onRuntimeInitialized = resolve;
            });

            generateBtn.disabled = false;
            status.textContent = "SHA-0 Ready!";

            generateBtn.addEventListener('click', () => {
                const text = inputText.value.trim();
                if (!text) {
                    status.textContent = "Enter text first!";
                    return;
                }

                try {
                    status.textContent = "Computing hash...";
                    const hash = Module.ccall(
                        'sha0_hash_wasm',
                        'string',
                        ['string'],
                        [text]
                    );
                    outputHash.value = hash;
                    status.textContent = "Hash generated!";
                } catch (error) {
                    status.textContent = "Error: " + error.message;
                    console.error(error);
                }
            });
        };

        script.onerror = () => {
            status.textContent = "Failed to load sha0.js";
            console.error("sha0.js failed to load");
        };

    } catch (error) {
        status.textContent = "Error: " + error.message;
        console.error(error);
    }
});