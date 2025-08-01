document.addEventListener('DOMContentLoaded', async () => {
    const inputText = document.getElementById('inputText');
    const generateBtn = document.getElementById('generateBtn');
    const outputHash = document.getElementById('outputHash');
    const status = document.getElementById('status');

    status.textContent = "Loading SHA-0...";
    generateBtn.disabled = true;

    // Wait for Emscripten to initialize
    Module = {};
    Module.onRuntimeInitialized = () => {
        generateBtn.disabled = false;
        status.textContent = "SHA-0 Ready!";
    };

    generateBtn.addEventListener('click', () => {
        const text = inputText.value.trim();
        if (!text) {
            status.textContent = "Please enter text first!";
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
});
