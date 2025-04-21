const ESP_IP = "http://192.168.0.109"; // Set your ESP's actual IP address

document.getElementById('alienForm').addEventListener('submit', function (event) {
  event.preventDefault();

  const message = document.getElementById('msgInput').value.trim();
  const count = parseInt(document.getElementById('repeatInput').value);

  const output = document.getElementById('outputText');

  // Validate input
  if (message === "" || isNaN(count) || count < 1) {
    output.innerText = "Please enter a message and valid repeat count.";
    return;
  }

  // Show loading message
  output.innerText = "Sending to space... 👽📡";

  fetch(`${ESP_IP}/submit?msg=${encodeURIComponent(message)}&count=${count}`)
    .then(response => response.text())
    .then(data => {
      if (data.includes("Message received:")) {
        output.innerText = `✅ Sent: "${message}" x${count}`;
      } else {
        output.innerText = "⚠️ Message failed to send. ESP might be busy.";
      }
    })
    .catch(err => {
      output.innerText = "🚨 Network error. Check ESP IP or WiFi.";
      console.error(err);
    });
});

// Combined placeholder messages
const placeholders = [
  "Type something fun 🎉",
  "What's on your mind? 🤔",
  "LCD needs love too 💬",
  "Keep it short & sweet 🍬",
  "Say hello to the matrix 👋",
  "Do aliens get impostor syndrome? 👽",
  "If trees WiFi, do forests buffer? 🌳📶",
  "Is the universe Tinder for stars? 💫❤️",
  "Why do we park in driveways? 🚗❓",
  "Do black holes need dark matter? 🕳️🌌",
  "If milk expires, does that mean cheese is immortal? 🧀👑",
  "Do parallel universes have parallel parking? 🚗🌌",
  "Is 'adulting' just cosmic DLC? 👩💼🎮",
  "Why don't skeletons fight? (No guts) 💀⚔️",
  "If I'm made of stardust...am I allergic to myself? 🌟🤧",
  "Do astronauts call space 'upstairs'? 🚀🔭",
  "Is math related to science? ➕🔬",
  "If I eat a clock, is that time consuming? ⏰🍽️",
  "Why is it called 'buildings' if they're already built? 🏢❓",
];

let currentIndex = 0;
setInterval(() => {
  document.getElementById("msgInput").placeholder = placeholders[currentIndex];
  currentIndex = (currentIndex + 1) % placeholders.length;
}, 3000);

// Cosmic dice fortunes
function luckyDice() {
  const result = Math.floor(Math.random() * 10) + 1;
  const cosmicFortunes = [
    "🎲 1: Your quantum particles will align with pizza delivery timelines! 🍕⏳",
    "🎲 2: Alien cats approve your life choices! 🐈👽",
    "🎲 3: Parallel-you is eating tacos right now! 🌮🌌",
    "🎲 4: Gravitational waves suggest...nap time! 😴🌊",
    "🎲 5: Dark matter says: Treat yourself! 🍩🌑",
    "🎲 6: Space-time continuum wants you to meme harder! 🐸🚀",
    "🎲 7: Interstellar wifi connected! Password: 'banana' 📡🍌",
    "🎲 8: Nebula forecast: 100% chance of dad jokes! ☁️😆",
    "🎲 9: Black hole detected...in your snack drawer! 🕳️🍪",
    "🎲 10: Cosmic verdict: You're officially awesome! 🌟👩🚀",
  ];

  const fortune = `${cosmicFortunes[result - 1]}`;
  typewriterEffect(fortune, "fortuneText");

  document.body.style.animation = "alienGlow 2s infinite";
  setTimeout(() => {
    document.body.style.animation = "";
  }, 2000);
}

// Typewriter effect with cursor
function typewriterEffect(text, targetId) {
  const target = document.getElementById(targetId);
  target.innerHTML = "";
  let i = 0;
  const interval = setInterval(() => {
    target.innerHTML =
      text.substring(0, i) + '<span class="input-cursor">|</span>';
    i++;
    if (i > text.length) {
      clearInterval(interval);
      target.innerHTML = text;
    }
  }, 50);
}
