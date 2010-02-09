require 'test/unit'
require 'rbclips'

class Test_Base < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Base)
  end

  def test_insert_command
    # Only Strings are allowed
    assert_raise(Clips::ArgumentError)  { Clips::Base.insert_command 5 }
    
    assert_equal "<Fact-0>\n", Clips::Base.insert_command("(assert (a b))")
    assert_equal "<Fact-1>\n", Clips::Base.insert_command("(assert (c d))")
    assert_equal "<Fact-2>\n", Clips::Base.insert_command("(assert (e f))")
    assert_equal "f-0     (a b)\nf-1     (c d)\nf-2     (e f)\nFor a total of 3 facts.\n", 
                Clips::Base.insert_command("(facts)")
    assert_equal "", Clips::Base.insert_command("(clear)")
    assert_equal "", Clips::Base.insert_command("(facts)")
    assert_raise(Clips::Exception) { Clips::Base.insert_command("(microsoft on mars)") }
    assert_raise(Clips::Exception) { Clips::Base.insert_command("(microsoft on mars") }
  end

  def test_run
    assert_raise(Clips::ArgumentError)  { Clips::Base.run "ahoj" }
    assert_raise(Clips::ArgumentError)  { Clips::Base.run 3.5  }

    assert_equal 0, Clips::Base.run(1)
    assert_equal 0, Clips::Base.run(-1)
    assert_equal 0, Clips::Base.run
  end
end
