const placeholders = [
    "Type something fun 🎉",
    "What's on your mind? 🤔",
    "LCD needs love too 💬",
    "Keep it short & sweet 🍬",
    "Say hello to the matrix 👋"
  ];
  
  let currentIndex = 0;
  setInterval(() => {
    document.getElementById("msgInput").placeholder = placeholders[currentIndex];
    currentIndex = (currentIndex + 1) % placeholders.length;
  }, 3000);
  
  function typewriterEffect(text, targetId) {
    const target = document.getElementById(targetId);
    target.textContent = "";
    let i = 0;
    const interval = setInterval(() => {
      target.textContent += text.charAt(i);
      i++;
      if (i > text.length) clearInterval(interval);
    }, 70);
  }
  
  function sendMessage() {
    const msg = document.getElementById("msgInput").value.trim();
    if (msg === "") return;
  
    typewriterEffect("📤 Sending: " + msg, "outputText");
  
    const emojis = ["🎉", "📢", "✅", "💡", "🚀", "😎", "📺"];
    const randomEmoji = emojis[Math.floor(Math.random() * emojis.length)];
    document.getElementById("emojiArea").textContent = randomEmoji;
  
    document.getElementById("msgInput").value = "";
  }
  